#include "ads1115.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "ADS1115";

// ADS1115 register pointers
#define REG_CONVERSION  0x00
#define REG_CONFIG      0x01

// Config bit fields
#define CFG_OS_SINGLE     (1 << 15)
#define CFG_MUX_A2_A3     (3 << 12)   // AINP=AIN2, AINN=AIN3
#define CFG_MODE_SINGLE   (1 << 8)    // single-shot
#define CFG_COMP_DISABLE  0x0003

static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

static ads1115_pga_t cur_pga = ADS1115_PGA_2_048V;
static ads1115_dr_t  cur_dr  = ADS1115_DR_128SPS;

static const float pga_fs_mv[] = { 6144.0f, 4096.0f, 2048.0f, 1024.0f, 512.0f, 256.0f };

static esp_err_t write_config(void)
{
    uint16_t cfg = CFG_OS_SINGLE
                 | CFG_MUX_A2_A3
                 | ((uint16_t)cur_pga << 9)
                 | CFG_MODE_SINGLE
                 | ((uint16_t)cur_dr << 5)
                 | CFG_COMP_DISABLE;

    uint8_t buf[3] = { REG_CONFIG, (uint8_t)(cfg >> 8), (uint8_t)(cfg & 0xFF) };
    return i2c_master_transmit(dev_handle, buf, sizeof(buf), 100);
}

esp_err_t ads1115_init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = ADS1115_I2C_PORT,
        .scl_io_num = ADS1115_I2C_SCL_GPIO,
        .sda_io_num = ADS1115_I2C_SDA_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_cfg, &bus_handle), TAG, "bus init failed");

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADS1115_ADDR,
        .scl_speed_hz = ADS1115_I2C_FREQ_HZ,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle),
                        TAG, "add device failed");

    // Probe: send one config write; NACK means the chip is not there
    esp_err_t ret = write_config();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "ADS1115 not responding at 0x%02X (check wiring and level shifter)", ADS1115_ADDR);
        return ret;
    }

    ESP_LOGI(TAG, "init OK: SCL=%d SDA=%d addr=0x%02X",
             ADS1115_I2C_SCL_GPIO, ADS1115_I2C_SDA_GPIO, ADS1115_ADDR);
    return ESP_OK;
}

// One read attempt: start conversion, poll ready, read result
static esp_err_t read_once(int16_t *raw, float *voltage_mv)
{
    // Start single-shot conversion
    ESP_RETURN_ON_ERROR(write_config(), TAG, "start conversion failed");

    // Poll conversion-ready bit (OS bit in config goes high when done)
    bool ready = false;
    for (int i = 0; i < 50; i++)
    {
        uint8_t reg = REG_CONFIG;
        uint8_t cfg[2] = {0};
        ESP_RETURN_ON_ERROR(i2c_master_transmit_receive(dev_handle, &reg, 1, cfg, 2, 100),
                            TAG, "read config failed");
        if (cfg[0] & 0x80) { ready = true; break; }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    ESP_RETURN_ON_FALSE(ready, ESP_ERR_TIMEOUT, TAG, "conversion timeout");

    // Read conversion register
    uint8_t reg = REG_CONVERSION;
    uint8_t data[2] = {0};
    ESP_RETURN_ON_ERROR(i2c_master_transmit_receive(dev_handle, &reg, 1, data, 2, 100),
                        TAG, "read conversion failed");

    *raw = (int16_t)((data[0] << 8) | data[1]);

    if (voltage_mv)
    {
        *voltage_mv = (float)(*raw) * pga_fs_mv[(int)cur_pga] / 32768.0f;
    }
    return ESP_OK;
}

esp_err_t ads1115_read_diff_2_3(int16_t *raw, float *voltage_mv)
{
    ESP_RETURN_ON_FALSE(raw != NULL, ESP_ERR_INVALID_ARG, TAG, "raw is NULL");

    // Retry a few times: the IDF driver resets the bus after a failed
    // transaction, so a retry usually recovers from a marginal-bus glitch
    esp_err_t ret = ESP_FAIL;
    for (int attempt = 0; attempt < 3; attempt++)
    {
        ret = read_once(raw, voltage_mv);
        if (ret == ESP_OK)
            return ESP_OK;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return ret;
}

// ── Auto-PGA acquisition ───────────────────────────────────────
// Take N samples, look at the peak, step the PGA up (larger FS) if near
// saturation or down (smaller FS) if there is lots of headroom, then average
// the N samples at the settled PGA. Averaging on the raw count keeps full
// resolution; the mV conversion is done once at the end.
#define AVG_N            8        // samples per measurement
#define AVG_MAX_STEPS    6        // bound on PGA re-ranging attempts
#define SAT_HI_FRAC      0.93f    // >= 93% FS  -> step to a larger  FS (less gain)
#define SAT_LO_FRAC      0.40f    // <  40% FS  -> step to a smaller FS (more gain)

esp_err_t ads1115_read_diff_avg8(float *voltage_mv, ads1115_pga_t *pga_used)
{
    ESP_RETURN_ON_FALSE(voltage_mv != NULL, ESP_ERR_INVALID_ARG, TAG, "voltage_mv is NULL");

    for (int step = 0; step < AVG_MAX_STEPS; step++)
    {
        int16_t  raw[AVG_N];
        int32_t  acc = 0;
        int16_t  max_abs = 0;
        bool     fail = false;

        for (int i = 0; i < AVG_N; i++)
        {
            int16_t r = 0;
            esp_err_t ret = read_once(&r, NULL);   // convert once at the end
            if (ret != ESP_OK)
            {
                ESP_LOGW(TAG, "sample %d/%d failed: %s", i + 1, AVG_N, esp_err_to_name(ret));
                fail = true;
                break;
            }
            raw[i] = r;
            acc   += r;
            int16_t a = (r < 0) ? (int16_t)(-r) : r;
            if (a > max_abs)
                max_abs = a;
        }
        if (fail)
            return ESP_FAIL;

        float frac = (float)max_abs / 32768.0f;

        // Near saturation -> back off to a larger full-scale range
        if (frac >= SAT_HI_FRAC && cur_pga > ADS1115_PGA_6_144V)
        {
            cur_pga = (ads1115_pga_t)(cur_pga - 1);
            write_config();
            ESP_LOGI(TAG, "PGA -> %d (FS %.0fmV), peak %.0f%%; re-ranging up", (int)cur_pga, pga_fs_mv[(int)cur_pga], frac * 100.0f);
            continue;
        }
        // Lots of headroom -> drop to a smaller range for finer resolution
        if (frac < SAT_LO_FRAC && cur_pga < ADS1115_PGA_0_256V)
        {
            cur_pga = (ads1115_pga_t)(cur_pga + 1);
            write_config();
            ESP_LOGI(TAG, "PGA -> %d (FS %.0fmV), peak %.0f%%; re-ranging down", (int)cur_pga, pga_fs_mv[(int)cur_pga], frac * 100.0f);
            continue;
        }

        // Settled: average the raw counts, convert once with this PGA's scale
        int16_t avg_raw = (int16_t)(acc / AVG_N);
        *voltage_mv = (float)avg_raw * pga_fs_mv[(int)cur_pga] / 32768.0f;
        if (pga_used)
            *pga_used = cur_pga;
        return ESP_OK;
    }

    ESP_LOGE(TAG, "auto-PGA did not settle within %d steps", AVG_MAX_STEPS);
    return ESP_ERR_TIMEOUT;
}

esp_err_t ads1115_set_pga(ads1115_pga_t pga)
{
    if (pga > ADS1115_PGA_0_256V)
        return ESP_ERR_INVALID_ARG;
    cur_pga = pga;
    return write_config();
}