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
#define CFG_MUX_A0_A1     (4 << 12)   // AINP=AIN0, AINN=AIN1
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
                 | CFG_MUX_A0_A1
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

esp_err_t ads1115_read_diff_0_1(int16_t *raw, float *voltage_mv)
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

esp_err_t ads1115_set_pga(ads1115_pga_t pga)
{
    if (pga > ADS1115_PGA_0_256V)
        return ESP_ERR_INVALID_ARG;
    cur_pga = pga;
    return write_config();
}