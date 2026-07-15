#include "scan_process.h"
#include "brain.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdlib.h>

static const char *TAG = "SCAN_PROCESS";

static int s_current_adc_value = ADC_MID_RESOLUTION;
static int s_signed_value = 0;
static int s_positive_arc_value = 0;
static int s_negative_arc_value = 0;
static int s_needle_angle = 0;
static int s_pulse_count = 0;
static bool s_rand_seeded = false;



static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static int map_adc_magnitude_to_percent(int magnitude)
{
    magnitude = clamp_int(magnitude, 0, ADC_MID_RESOLUTION);
    return (magnitude * 100) / ADC_MID_RESOLUTION;
}

static int map_adc_diff_to_needle_angle(int diff)
{
    diff = clamp_int(diff, -ADC_MID_RESOLUTION, ADC_MID_RESOLUTION);
    return (diff * SCAN_NEEDLE_MAX_ANGLE) / ADC_MID_RESOLUTION;
}

static void scan_process_calculate_display_values(void)
{
    int diff = s_current_adc_value - ADC_MID_RESOLUTION;
    int magnitude = diff >= 0 ? diff : -diff;

    s_signed_value = diff;
    s_needle_angle = map_adc_diff_to_needle_angle(diff);

    if (diff > 0) {
        s_positive_arc_value = map_adc_magnitude_to_percent(magnitude);
        s_negative_arc_value = 0;
    } else if (diff < 0) {
        s_positive_arc_value = 0;
        s_negative_arc_value = map_adc_magnitude_to_percent(magnitude);
    } else {
        s_positive_arc_value = 0;
        s_negative_arc_value = 0;
        s_needle_angle = 0;
    }
}

void scan_process_init(void)
{
    if (!s_rand_seeded) {
        srand(12345);
        s_rand_seeded = true;
    }

    s_current_adc_value = ADC_MID_RESOLUTION;
    s_pulse_count = 0;
    

    scan_process_calculate_display_values();

    ESP_LOGI(TAG, "Scan process started and initialized");
}

void scan_process_stop(void)
{
    //s_pulse_count = 0;
    
    ESP_LOGI(TAG, "Scan process stopped");
}

void scan_process_handle_trigger(scan_mode_t mode)
{
  

    //scan_mode_t mode = (scan_mode_t)brain_get_scan_mode();

    switch (mode) {
        case SCAN_MODE_MANPC:
        case SCAN_MODE_MANMEM:
            s_current_adc_value = rand() % (ADC_MAX_RESOLUTION + 1);
            s_pulse_count++;
            scan_process_calculate_display_values();
            ESP_LOGI(TAG, "Manual trigger: val=%d pulse=%d", s_signed_value, s_pulse_count);
            break;

        case SCAN_MODE_AUTOPC:
        case SCAN_MODE_AUTOMEM:
            ESP_LOGI(TAG, "Trigger ignored in auto mode");
            break;

        default:
            ESP_LOGW(TAG, "Unknown mode: %d", mode);
            break;
    }
}






int scan_process_get_current_adc_value(void)
{
    return s_current_adc_value;
}

int scan_process_get_signed_value(void)
{
    return s_signed_value;
}

int scan_process_get_positive_arc_value(void)
{
    return s_positive_arc_value;
}

int scan_process_get_negative_arc_value(void)
{
    return s_negative_arc_value;
}

int scan_process_get_needle_angle(void)
{
    return s_needle_angle;
}

int scan_process_get_pulse_count(void)
{
    return s_pulse_count;
}
