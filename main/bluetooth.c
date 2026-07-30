#include "bluetooth.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "brain.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"
#include "esp_spp_api.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/timers.h"

static const char *TAG = "BT";

static bool s_enabled = false;
static bool s_connected = false;
static uint32_t s_spp_handle = 0;
static char s_device_name[32] = "ESP32_SPP";

extern system_settings_t g_settings;

/* =========================
 *  TX queue (int32 packets)
 * ========================= */
#define BT_TX_QUEUE_SIZE 64

static int32_t s_bt_tx_queue[BT_TX_QUEUE_SIZE];
static uint16_t s_bt_tx_head = 0;
static uint16_t s_bt_tx_tail = 0;
static uint16_t s_bt_tx_count = 0;

static bool s_bt_tx_busy = false;

/* critical section for queue + busy flag + stream state */
static portMUX_TYPE s_bt_tx_mux = portMUX_INITIALIZER_UNLOCKED;

/* =========================
 *  Stream sender (array paced by WRITE_EVT)
 * ========================= */
static const int32_t *s_stream_data = NULL;
static size_t s_stream_count = 0;
static size_t s_stream_idx = 0;
static bool s_stream_active = false;   // stream in progress
static bool s_stream_waiting = false;  // one write is outstanding

static TimerHandle_t s_stream_retry_timer = NULL;

/* ====== Forward declarations ====== */
static void bt_tx_queue_clear(void);
static esp_err_t bt_send_next_queued_packet(void);

/* stream internal */
static esp_err_t bt_stream_try_send_locked(void);
static void bt_stream_stop_locked(void);
static void bt_stream_retry_timer_cb(TimerHandle_t xTimer);

/* =========================
 *  Helpers
 * ========================= */
static const char *gap_event_name(esp_bt_gap_cb_event_t event)
{
    switch (event) {
    case ESP_BT_GAP_PIN_REQ_EVT:   return "PIN_REQ";
    case ESP_BT_GAP_CFM_REQ_EVT:   return "CFM_REQ";
    case ESP_BT_GAP_KEY_NOTIF_EVT: return "KEY_NOTIF";
    case ESP_BT_GAP_KEY_REQ_EVT:   return "KEY_REQ";
    case ESP_BT_GAP_AUTH_CMPL_EVT: return "AUTH_CMPL";
    case ESP_BT_GAP_MODE_CHG_EVT:  return "MODE_CHG";
    default:                       return "OTHER";
    }
}

static void bt_build_pin(char *out_pin, size_t out_size, int pass)
{
    if (out_pin == NULL || out_size < 5) return;

    if (pass < 0) pass = 0;
    if (pass > 9999) pass = 9999;

    snprintf(out_pin, out_size, "%04d", pass);
}

/* =========================
 *  GAP callback
 * ========================= */
static void bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    //ESP_LOGI(TAG, "GAP event %d (%s)", event, gap_event_name(event));

    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "BT GAP auth success, device: %s", param->auth_cmpl.device_name);
        } else {
            ESP_LOGE(TAG, "BT GAP auth failed, status=%d", param->auth_cmpl.stat);
        }
        break;

    case ESP_BT_GAP_PIN_REQ_EVT: {
        char pin_str[5] = {0};
        esp_bt_pin_code_t pin_code = {0};

        bt_build_pin(pin_str, sizeof(pin_str), g_settings.bl_pass);
        memcpy(pin_code, pin_str, 4);

        //ESP_LOGI(TAG, "BT GAP PIN requested. Responding with: %s", pin_str);
        esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        break;
    }

    case ESP_BT_GAP_MODE_CHG_EVT:
        //ESP_LOGI(TAG, "BT GAP mode changed: mode=%d", param->mode_chg.mode);
        break;

    default:
        break;
    }
}

/* =========================
 *  TX Queue functions
 * ========================= */
static inline bool bt_tx_queue_is_empty_unsafe(void) { return s_bt_tx_count == 0; }
static inline bool bt_tx_queue_is_full_unsafe(void)  { return s_bt_tx_count >= BT_TX_QUEUE_SIZE; }

static bool bt_tx_queue_push(int32_t value)
{
    bool ok = false;

    portENTER_CRITICAL(&s_bt_tx_mux);
    if (!bt_tx_queue_is_full_unsafe()) {
        s_bt_tx_queue[s_bt_tx_tail] = value;
        s_bt_tx_tail = (s_bt_tx_tail + 1) % BT_TX_QUEUE_SIZE;
        s_bt_tx_count++;
        ok = true;
    }
    portEXIT_CRITICAL(&s_bt_tx_mux);

    return ok;
}

static bool bt_tx_queue_pop(int32_t *value)
{
    bool ok = false;
    if (value == NULL) return false;

    portENTER_CRITICAL(&s_bt_tx_mux);
    if (!bt_tx_queue_is_empty_unsafe()) {
        *value = s_bt_tx_queue[s_bt_tx_head];
        s_bt_tx_head = (s_bt_tx_head + 1) % BT_TX_QUEUE_SIZE;
        s_bt_tx_count--;
        ok = true;
    }
    portEXIT_CRITICAL(&s_bt_tx_mux);

    return ok;
}

static void bt_tx_queue_clear(void)
{
    portENTER_CRITICAL(&s_bt_tx_mux);
    s_bt_tx_head = 0;
    s_bt_tx_tail = 0;
    s_bt_tx_count = 0;
    s_bt_tx_busy  = false;
    portEXIT_CRITICAL(&s_bt_tx_mux);
}

/* =====================================
 * Send exactly ONE queued packet if idle
 * called from:
 *  - bluetooth_send_int32()
 *  - ESP_SPP_WRITE_EVT (when NOT streaming)
 *  - ESP_SPP_CONG_EVT (optional)
 * ===================================== */
static esp_err_t bt_send_next_queued_packet(void)
{
    /* fast state check */
    if (!s_enabled || !s_connected || s_spp_handle == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    /* if stream is active, do not interleave queue packets */
    portENTER_CRITICAL(&s_bt_tx_mux);
    if (s_stream_active) {
        portEXIT_CRITICAL(&s_bt_tx_mux);
        return ESP_OK;
    }

    if (s_bt_tx_busy) {
        portEXIT_CRITICAL(&s_bt_tx_mux);
        return ESP_OK;
    }
    s_bt_tx_busy = true;
    portEXIT_CRITICAL(&s_bt_tx_mux);

    int32_t value;
    if (!bt_tx_queue_pop(&value)) {
        portENTER_CRITICAL(&s_bt_tx_mux);
        s_bt_tx_busy = false;
        portEXIT_CRITICAL(&s_bt_tx_mux);
        return ESP_OK;
    }

    char tx_str[32];
    int len = snprintf(tx_str, sizeof(tx_str), "%ld\r\n", (long)value);

    esp_err_t err = esp_spp_write(s_spp_handle, len, (uint8_t *)tx_str);
    if (err != ESP_OK) {
        portENTER_CRITICAL(&s_bt_tx_mux);
        s_bt_tx_busy = false;
        portEXIT_CRITICAL(&s_bt_tx_mux);

        ESP_LOGE(TAG, "esp_spp_write failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

/* =========================
 *  Stream internals
 * ========================= */
static void bt_stream_stop_locked(void)
{
    s_stream_active = false;
    s_stream_waiting = false;
    s_stream_data = NULL;
    s_stream_count = 0;
    s_stream_idx = 0;
}

static void bt_stream_retry_timer_cb(TimerHandle_t xTimer)
{
    (void)xTimer;

    portENTER_CRITICAL(&s_bt_tx_mux);
    if (!s_stream_active) {
        portEXIT_CRITICAL(&s_bt_tx_mux);
        return;
    }

    /* retry only if we are not waiting for a previous WRITE_EVT */
    if (!s_stream_waiting) {
        (void)bt_stream_try_send_locked();
    }
    portEXIT_CRITICAL(&s_bt_tx_mux);
}

/* Send current stream idx. Must be called with mux held. */
static esp_err_t bt_stream_try_send_locked(void)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) return ESP_ERR_INVALID_STATE;
    if (!s_stream_active) return ESP_OK;

    if (s_stream_waiting) return ESP_OK;

    if (s_stream_idx >= s_stream_count) {
        //ESP_LOGI(TAG, "Stream finished. count=%u", (unsigned)s_stream_count);
        bt_stream_stop_locked();
        return ESP_OK;
    }

    /* format current item */
    char tx_str[32];
    int len = snprintf(tx_str, sizeof(tx_str), "%ld\r\n", (long)s_stream_data[s_stream_idx]);

    esp_err_t err = esp_spp_write(s_spp_handle, len, (uint8_t *)tx_str);
    if (err == ESP_OK) {
        s_stream_waiting = true; /* wait for WRITE_EVT */
    } else {
        // ESP_LOGW(TAG, "stream esp_spp_write err=%s (idx=%u). retry scheduled",
        //          esp_err_to_name(err), (unsigned)s_stream_idx);

        s_stream_waiting = false;
        if (s_stream_retry_timer) {
            xTimerStart(s_stream_retry_timer, 0);
        }
    }
    return err;
}

/* =========================
 *  SPP callback
 * ========================= */
static void bt_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        //ESP_LOGI(TAG, "SPP init done");
        esp_spp_start_srv(
            ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT,
            ESP_SPP_ROLE_SLAVE,
            0,
            "SPP_SERVER");
        break;

    case ESP_SPP_START_EVT:
       // ESP_LOGI(TAG, "SPP server started");
        break;

    case ESP_SPP_SRV_OPEN_EVT:
        s_connected = true;
        s_spp_handle = param->srv_open.handle;

        bt_tx_queue_clear();

        portENTER_CRITICAL(&s_bt_tx_mux);
        bt_stream_stop_locked(); /* ensure no stale stream */
        portEXIT_CRITICAL(&s_bt_tx_mux);

        //ESP_LOGI(TAG, "SPP client connected, handle=%u", (unsigned)s_spp_handle);
        break;

    case ESP_SPP_CLOSE_EVT:
        s_connected = false;
        s_spp_handle = 0;

        bt_tx_queue_clear();

        portENTER_CRITICAL(&s_bt_tx_mux);
        bt_stream_stop_locked();
        portEXIT_CRITICAL(&s_bt_tx_mux);

        //ESP_LOGI(TAG, "SPP connection closed");
        break;

    case ESP_SPP_WRITE_EVT: {
        //ESP_LOGI(TAG, "Write completed, status=%d, len=%d", param->write.status, param->write.len);

        /* 1) If a stream is active, it has priority and is paced here */
        portENTER_CRITICAL(&s_bt_tx_mux);
        if (s_stream_active) {
            /* write completed for stream */
            s_stream_waiting = false;

            if (param->write.status == ESP_SPP_SUCCESS) {
                /* advance only on success */
                s_stream_idx++;
                (void)bt_stream_try_send_locked();
            } else {
                /* do NOT advance; retry same idx later */
                //ESP_LOGW(TAG, "Stream write failed status=%d, retry idx=%u",
                        //param->write.status, (unsigned)s_stream_idx);

                if (s_stream_retry_timer) {
                    xTimerStart(s_stream_retry_timer, 0);
                }
            }
            portEXIT_CRITICAL(&s_bt_tx_mux);
            break;
        }
        portEXIT_CRITICAL(&s_bt_tx_mux);

        /* 2) Otherwise handle normal queued int32 packets */
        portENTER_CRITICAL(&s_bt_tx_mux);
        s_bt_tx_busy = false;
        portEXIT_CRITICAL(&s_bt_tx_mux);

        if (param->write.status == ESP_SPP_SUCCESS) {
            (void)bt_send_next_queued_packet();
        } else {
            /* on congestion/fail: don't drain queue aggressively */
            //ESP_LOGW(TAG, "Queue write failed status=%d, waiting for CONG_EVT/uncongest",
                     //param->write.status);
        }
        break;
    }

    case ESP_SPP_CONG_EVT:
        //ESP_LOGW(TAG, "SPP Congestion event: cong=%d", param->cong.cong);

        if (!param->cong.cong) {
            /* congestion cleared: try to resume stream first, else queue */
            portENTER_CRITICAL(&s_bt_tx_mux);
            if (s_stream_active && !s_stream_waiting) {
                (void)bt_stream_try_send_locked();
                portEXIT_CRITICAL(&s_bt_tx_mux);
                break;
            }
            portEXIT_CRITICAL(&s_bt_tx_mux);

            (void)bt_send_next_queued_packet();
        }
        break;

    default:
        break;
    }
}

/* =========================
 *  Public API
 * ========================= */
esp_err_t bluetooth_init(const char *device_name)
{
    if (device_name != NULL && device_name[0] != '\0') {
        strncpy(s_device_name, device_name, sizeof(s_device_name) - 1);
        s_device_name[sizeof(s_device_name) - 1] = '\0';
    }
    return ESP_OK;
}

esp_err_t bluetooth_enable(void)
{
    if (s_enabled) return ESP_OK;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Release BLE memory */
    esp_bt_controller_mem_release(ESP_BT_MODE_BLE);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_controller_init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_controller_enable failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    bluedroid_cfg.ssp_en = false; /* enforce legacy PIN */

    ret = esp_bluedroid_init_with_cfg(&bluedroid_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bluedroid_init_with_cfg failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bluedroid_enable failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bt_gap_register_callback(bt_gap_cb);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_register_callback failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_spp_register_callback(bt_spp_cb);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_spp_register_callback failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_spp_cfg_t spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = true,
        .tx_buffer_size = 0, /* default */
    };

    ret = esp_spp_enhanced_init(&spp_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_spp_enhanced_init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    if (g_settings.bl_name[0] != '\0') {
        strncpy(s_device_name, g_settings.bl_name, sizeof(s_device_name) - 1);
        s_device_name[sizeof(s_device_name) - 1] = '\0';
    }

    ret = esp_bt_gap_set_device_name(s_device_name);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_set_device_name failed: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_bt_pin_code_t dummy_pin;
    ret = esp_bt_gap_set_pin(ESP_BT_PIN_TYPE_VARIABLE, 0, dummy_pin);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_set_pin failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_bt_gap_set_scan_mode failed: %s", esp_err_to_name(ret));
        return ret;
    }

    /* create stream retry timer once */
    if (!s_stream_retry_timer) {
        s_stream_retry_timer = xTimerCreate("bt_str_retry", pdMS_TO_TICKS(30), pdFALSE, NULL, bt_stream_retry_timer_cb);
        if (!s_stream_retry_timer) {
            ESP_LOGE(TAG, "Failed to create stream retry timer");
            return ESP_ERR_NO_MEM;
        }
    }

    s_enabled = true;
    s_connected = false;
    s_spp_handle = 0;
    bt_tx_queue_clear();

    portENTER_CRITICAL(&s_bt_tx_mux);
    bt_stream_stop_locked();
    portEXIT_CRITICAL(&s_bt_tx_mux);

    ESP_LOGI(TAG, "Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.");
    ESP_LOGI(TAG, "Device Name: %s", s_device_name);

    return ESP_OK;
}

esp_err_t bluetooth_disable(void)
{
    if (!s_enabled) return ESP_OK;

    s_connected = false;
    s_spp_handle = 0;
    bt_tx_queue_clear();

    portENTER_CRITICAL(&s_bt_tx_mux);
    bt_stream_stop_locked();
    portEXIT_CRITICAL(&s_bt_tx_mux);

    esp_err_t ret;

    ret = esp_spp_deinit();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_spp_deinit: %s", esp_err_to_name(ret));
    }

    ret = esp_bluedroid_disable();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_bluedroid_disable: %s", esp_err_to_name(ret));
    }

    ret = esp_bluedroid_deinit();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_bluedroid_deinit: %s", esp_err_to_name(ret));
    }

    ret = esp_bt_controller_disable();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_bt_controller_disable: %s", esp_err_to_name(ret));
    }

    ret = esp_bt_controller_deinit();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "esp_bt_controller_deinit: %s", esp_err_to_name(ret));
    }

    s_enabled = false;
    ESP_LOGI(TAG, "Bluetooth disabled");

    return ESP_OK;
}

bool bluetooth_is_enabled(void)   { return s_enabled; }
bool bluetooth_is_connected(void) { return s_connected; }

/* NOTE:
 * raw write is still direct.
 */
esp_err_t bluetooth_send_raw(const uint8_t *data, size_t len)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) return ESP_ERR_INVALID_STATE;
    if (data == NULL || len == 0) return ESP_ERR_INVALID_ARG;

    return esp_spp_write(s_spp_handle, len, (uint8_t *)data);
}

/* paced by ESP_SPP_WRITE_EVT via queue */
esp_err_t bluetooth_send_int32(int32_t value)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) return ESP_ERR_INVALID_STATE;

    if (!bt_tx_queue_push(value)) {
        ESP_LOGW(TAG, "BT TX queue full, value dropped: %ld", (long)value);
        return ESP_ERR_NO_MEM;
    }

    return bt_send_next_queued_packet();
}

/* direct comma separated packet (kept as your old behavior) */
esp_err_t bluetooth_send_array(const int32_t *data, size_t count)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) return ESP_ERR_INVALID_STATE;
    if (data == NULL || count == 0) return ESP_ERR_INVALID_ARG;

    char buffer[256];
    int offset = 0;

    for (size_t i = 0; i < count; i++) {
        int written = snprintf(buffer + offset, sizeof(buffer) - offset,
                               (i == 0) ? "%ld" : ",%ld",
                               (long)data[i]);

        if (written < 0 || written >= (int)(sizeof(buffer) - offset)) {
            return ESP_ERR_NO_MEM;
        }
        offset += written;
    }

    if (offset + 2 >= (int)sizeof(buffer)) return ESP_ERR_NO_MEM;

    buffer[offset++] = '\r';
    buffer[offset++] = '\n';

    return esp_spp_write(s_spp_handle, offset, (uint8_t *)buffer);
}

/* =========================================================
 * NEW: paced stream sender for variable-length int32 arrays
 * - sends items one-by-one as "%ld\r\n"
 * - next item only after ESP_SPP_WRITE_EVT SUCCESS
 * - on congestion/fail: retries same item (no drop)
 * ========================================================= */
esp_err_t bluetooth_send_int32_stream_begin(const int32_t *data, size_t count)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) return ESP_ERR_INVALID_STATE;
    if (data == NULL || count == 0) return ESP_ERR_INVALID_ARG;

    /* prevent starting stream if queue is currently busy sending */
    portENTER_CRITICAL(&s_bt_tx_mux);
    if (s_stream_active) {
        portEXIT_CRITICAL(&s_bt_tx_mux);
        return ESP_ERR_INVALID_STATE; /* stream already running */
    }

    /* Optional policy: don't start stream if queued TX is in progress */
    if (s_bt_tx_busy || s_bt_tx_count > 0) {
        portEXIT_CRITICAL(&s_bt_tx_mux);
        return ESP_ERR_INVALID_STATE;
    }

    s_stream_data = data;
    s_stream_count = count;
    s_stream_idx = 0;
    s_stream_active = true;
    s_stream_waiting = false;

    /* kick first send */
    esp_err_t err = bt_stream_try_send_locked();
    portEXIT_CRITICAL(&s_bt_tx_mux);

    return err;
}

bool bluetooth_stream_is_active(void)
{
    bool active;
    portENTER_CRITICAL(&s_bt_tx_mux);
    active = s_stream_active;
    portEXIT_CRITICAL(&s_bt_tx_mux);
    return active;
}

void bluetooth_stream_abort(void)
{
    portENTER_CRITICAL(&s_bt_tx_mux);
    bt_stream_stop_locked();
    portEXIT_CRITICAL(&s_bt_tx_mux);
}
