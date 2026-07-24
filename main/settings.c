#include "settings.h"

#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "settings";
static const char *NVS_NS = "app_cfg";

app_settings_t g_settings;

static void settings_write_defaults_to_ram(void)
{
    g_settings.autocal_pls     = 16;
    g_settings.puls_max        = 300;
    g_settings.delay_time      = 500;     // 5.00s
    g_settings.stop_trg        = false;
    g_settings.beep            = true;
    g_settings.bl_auto_off     = false;
    g_settings.bl_auto_connect = false;
    g_settings.bl_pass         = 1234;
    strncpy(g_settings.bl_name, "MAGI_ESP", sizeof(g_settings.bl_name) - 1);
    g_settings.bl_name[sizeof(g_settings.bl_name) - 1] = '\0';
}

void settings_set_defaults(void)
{
    settings_write_defaults_to_ram();
}

void settings_load(void)
{
    settings_write_defaults_to_ram();

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NS, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "NVS open readonly failed, using defaults: %s", esp_err_to_name(err));
        return;
    }

    size_t required_size = sizeof(g_settings);
    err = nvs_get_blob(handle, "settings", &g_settings, &required_size);
    nvs_close(handle);

    if (err != ESP_OK || required_size != sizeof(g_settings)) {
        ESP_LOGW(TAG, "Load failed, using defaults: %s", esp_err_to_name(err));
        settings_write_defaults_to_ram();
        return;
    }

    ESP_LOGI(TAG, "Settings loaded from NVS");
}

void settings_save(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS open write failed: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_blob(handle, "settings", &g_settings, sizeof(g_settings));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Save failed: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Settings saved to NVS");
    }
}
