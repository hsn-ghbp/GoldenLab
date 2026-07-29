#include "app_settings.h"

#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "app_settings";

#define APP_SETTINGS_NVS_NAMESPACE "app_cfg"
#define APP_SETTINGS_NVS_KEY_DATA  "settings"
#define APP_SETTINGS_NVS_KEY_VER   "version"
#define APP_SETTINGS_VERSION       1U

app_settings_t g_app_settings;

static esp_err_t app_settings_nvs_init_once(void)
{
    static bool initialized = false;
    esp_err_t err;

    if (initialized) {
        return ESP_OK;
    }

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS requires erase, reinitializing");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err == ESP_OK) {
        initialized = true;
    }

    return err;
}

void app_settings_defaults(app_settings_t *settings)
{
    if (settings == NULL) {
        return;
    }

    memset(settings, 0, sizeof(*settings));

    settings->auto_calibration_enabled = true;
    settings->auto_calibration_pulse_count = 16;
    settings->max_pulses_per_sampling = 200;
    settings->auto_pulse_delay_ms = 1000;
    settings->trigger_stop_enabled_in_auto = true;
    settings->beep_after_each_pulse = true;
    settings->bluetooth_auto_connect = true;
    settings->auto_power_enabled = true;

    strncpy(settings->bluetooth_name, "ESP32_Device", sizeof(settings->bluetooth_name) - 1);
    strncpy(settings->bluetooth_password, "33138", sizeof(settings->bluetooth_password) - 1);
}

esp_err_t app_settings_save(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = app_settings_nvs_init_once();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NVS: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_open(APP_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_u32(nvs_handle, APP_SETTINGS_NVS_KEY_VER, APP_SETTINGS_VERSION);
    if (err == ESP_OK) {
        err = nvs_set_blob(
            nvs_handle,
            APP_SETTINGS_NVS_KEY_DATA,
            &g_app_settings,
            sizeof(g_app_settings)
        );
    }

    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
    }

    nvs_close(nvs_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save settings: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Settings saved");
    }

    return err;
}

esp_err_t app_settings_load(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    uint32_t version = 0;
    size_t required_size = sizeof(g_app_settings);

    err = app_settings_nvs_init_once();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NVS: %s", esp_err_to_name(err));
        app_settings_defaults(&g_app_settings);
        return err;
    }

    app_settings_defaults(&g_app_settings);

    err = nvs_open(APP_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "NVS namespace not available, using defaults");
        return app_settings_save();
    }

    err = nvs_get_u32(nvs_handle, APP_SETTINGS_NVS_KEY_VER, &version);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "No saved settings version found, writing defaults");
        nvs_close(nvs_handle);
        return app_settings_save();
    }
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed reading version, using defaults");
        nvs_close(nvs_handle);
        return err;
    }

    if (version != APP_SETTINGS_VERSION) {
        ESP_LOGW(TAG, "Settings version mismatch (%lu), resetting to defaults", (unsigned long)version);
        nvs_close(nvs_handle);
        return app_settings_save();
    }

    err = nvs_get_blob(
        nvs_handle,
        APP_SETTINGS_NVS_KEY_DATA,
        &g_app_settings,
        &required_size
    );
    nvs_close(nvs_handle);

    if (err == ESP_ERR_NVS_NOT_FOUND || required_size != sizeof(g_app_settings)) {
        ESP_LOGW(TAG, "Saved settings missing or invalid size, rewriting defaults");
        app_settings_defaults(&g_app_settings);
        return app_settings_save();
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed loading settings: %s", esp_err_to_name(err));
        app_settings_defaults(&g_app_settings);
        return err;
    }

    g_app_settings.bluetooth_name[sizeof(g_app_settings.bluetooth_name) - 1] = '\0';
    g_app_settings.bluetooth_password[sizeof(g_app_settings.bluetooth_password) - 1] = '\0';

    ESP_LOGI(TAG, "Settings loaded");
    return ESP_OK;
}

esp_err_t app_settings_reset(void)
{
    app_settings_defaults(&g_app_settings);
    return app_settings_save();
}
