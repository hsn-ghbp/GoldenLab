#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    bool auto_calibration_enabled;
    uint16_t auto_calibration_pulse_count;
    uint16_t max_pulses_per_sampling;
    uint16_t auto_pulse_delay_ms;
    bool trigger_stop_enabled_in_auto;
    bool beep_after_each_pulse;
    char bluetooth_name[32];
    char bluetooth_password[32];
    bool bluetooth_auto_connect;
    bool auto_power_enabled;
} app_settings_t;

extern app_settings_t g_app_settings;

void app_settings_defaults(app_settings_t *settings);
esp_err_t app_settings_load(void);
esp_err_t app_settings_save(void);
esp_err_t app_settings_reset(void);

#ifdef __cplusplus
}
#endif

#endif
