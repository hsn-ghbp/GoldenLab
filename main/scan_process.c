#include "scan_process.h"

#include "brain.h"
#include "esp_log.h"

static const char *TAG = "SCAN_PROCESS";

void scan_process_init(void)
{
    ESP_LOGI(TAG, "scan_process initialized");
}

void scan_process_handle_trigger(void)
{
    scan_mode_t mode = brain_get_scan_mode();

    switch (mode) {
        case SCAN_MODE_MANPC:
        case SCAN_MODE_MANMEM:
            ESP_LOGI(TAG, "Manual trigger: single sample requested, mode=%d", mode);
            break;

        case SCAN_MODE_AUTOPC:
        case SCAN_MODE_AUTOMEM:
            ESP_LOGI(TAG, "Auto trigger: start sampling requested, mode=%d", mode);
            break;

        default:
            ESP_LOGW(TAG, "Unknown scan mode: %d", mode);
            break;
    }
}

void scan_process_stop(void)
{
    ESP_LOGI(TAG, "Stop sampling requested");
}
