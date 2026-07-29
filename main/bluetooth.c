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

static const char *TAG = "BT";

static bool s_enabled = false;
static bool s_connected = false;
static uint32_t s_spp_handle = 0;
static char s_device_name[32] = "ESP32_SPP";

extern system_settings_t g_settings;

static const char *gap_event_name(esp_bt_gap_cb_event_t event)
{
    switch (event) {
    case ESP_BT_GAP_PIN_REQ_EVT:
        return "PIN_REQ";
    case ESP_BT_GAP_CFM_REQ_EVT:
        return "CFM_REQ";
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        return "KEY_NOTIF";
    case ESP_BT_GAP_KEY_REQ_EVT:
        return "KEY_REQ";
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        return "AUTH_CMPL";
    case ESP_BT_GAP_MODE_CHG_EVT:
        return "MODE_CHG";
    default:
        return "OTHER";
    }
}

static void bt_build_pin(char *out_pin, size_t out_size, int pass)
{
    if (out_pin == NULL || out_size < 5) {
        return;
    }

    if (pass < 0) {
        pass = 0;
    }
    if (pass > 9999) {
        pass = 9999;
    }

    snprintf(out_pin, out_size, "%04d", pass);
}

static void bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    ESP_LOGI(TAG, "GAP event %d (%s)", event, gap_event_name(event));

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

        ESP_LOGI(TAG, "BT GAP PIN requested. Responding with: %s", pin_str);
        esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        break;
    }

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(TAG, "BT GAP mode changed: mode=%d", param->mode_chg.mode);
        break;

    default:
        break;
    }
}

static void bt_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(TAG, "SPP init done");
        esp_spp_start_srv(
            ESP_SPP_SEC_AUTHENTICATE | ESP_SPP_SEC_ENCRYPT,
            ESP_SPP_ROLE_SLAVE,
            0,
            "SPP_SERVER");
        break;

    case ESP_SPP_START_EVT:
        ESP_LOGI(TAG, "SPP server started");
        break;

    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(TAG, "SPP client connected, handle=%lu",
                 (unsigned long)param->srv_open.handle);
        s_connected = true;
        s_spp_handle = param->srv_open.handle;
        break;

    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(TAG, "SPP client disconnected");
        s_connected = false;
        s_spp_handle = 0;
        break;

    case ESP_SPP_WRITE_EVT:
        ESP_LOGD(TAG, "SPP write complete");
        break;

    default:
        break;
    }
}

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
    if (s_enabled) {
        return ESP_OK;
    }

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // خاموش کردن BLE برای آزاد سازی منابع بر اساس استاندارد مثال رسمی
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

    // کپی کردن ساختار پیش‌فرض تنظیمات بلودروید و خاموش کردن کامل SSP
    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    bluedroid_cfg.ssp_en = false; // غیرفعال کردن پین خودکار SSP برای اجبار به پین عددی

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
        .tx_buffer_size = 0,
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

    // مقداردهی پین به صورت متغیر مشابه رفتار مثال رسمی برای Legacy Pairing
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

    s_enabled = true;
    s_connected = false;
    s_spp_handle = 0;

    ESP_LOGI(TAG, "Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.");
    ESP_LOGI(TAG, "Device Name: %s", s_device_name);

    return ESP_OK;
}

esp_err_t bluetooth_disable(void)
{
    if (!s_enabled) {
        return ESP_OK;
    }

    s_connected = false;
    s_spp_handle = 0;

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

bool bluetooth_is_enabled(void)
{
    return s_enabled;
}

bool bluetooth_is_connected(void)
{
    return s_connected;
}

esp_err_t bluetooth_send_int32(int32_t value)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%ld\r\n", (long)value);
    if (len <= 0) {
        return ESP_FAIL;
    }

    return esp_spp_write(s_spp_handle, len, (uint8_t *)buffer);
}

esp_err_t bluetooth_send_array(const int32_t *data, size_t count)
{
    if (!s_enabled || !s_connected || s_spp_handle == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || count == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    char buffer[256];
    int offset = 0;

    for (size_t i = 0; i < count; i++) {
        int written = snprintf(
            buffer + offset,
            sizeof(buffer) - offset,
            (i == 0) ? "%ld" : ",%ld",
            (long)data[i]);

        if (written < 0 || written >= (int)(sizeof(buffer) - offset)) {
            return ESP_ERR_NO_MEM;
        }

        offset += written;
    }

    if (offset + 2 >= (int)sizeof(buffer)) {
        return ESP_ERR_NO_MEM;
    }

    buffer[offset++] = '\r';
    buffer[offset++] = '\n';

    return esp_spp_write(s_spp_handle, offset, (uint8_t *)buffer);
}
