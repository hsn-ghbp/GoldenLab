#include "scan_process.h"
#include "brain.h"
#include "esp_log.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "SCAN_PROCESS";

static int s_current_adc_value = ADC_MID_RESOLUTION;
static int s_signed_value = 0;
static int s_positive_arc_value = 0;
static int s_negative_arc_value = 0;
static int s_needle_angle = 0;
static int s_pulse_count = 0;
static bool s_rand_seeded = false;

// بافر موقت در رم برای نگهداری نقاط اسکن جاری تا قبل از فینالایز شدن
static int16_t s_temp_scan_buffer[MAX_SCAN_POINTS];
static uint16_t s_temp_point_count = 0;

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

void scan_process_clear_temp_buffer(void)
{
    s_temp_point_count = 0;
    memset(s_temp_scan_buffer, 0, sizeof(s_temp_scan_buffer));
}

bool scan_process_add_point_to_buffer(int16_t adc_val)
{
    if (s_temp_point_count >= MAX_SCAN_POINTS) {
        ESP_LOGW(TAG, "Temporary scan buffer is full!");
        return false;
    }
    s_temp_scan_buffer[s_temp_point_count++] = adc_val;
    return true;
}

const int16_t* scan_process_get_buffer_data(uint16_t *out_count)
{
    if (out_count) {
        *out_count = s_temp_point_count;
    }
    return s_temp_scan_buffer;
}

void scan_process_init(void)
{
    if (!s_rand_seeded) {
        srand(12345);
        s_rand_seeded = true;
    }

    s_current_adc_value = ADC_MID_RESOLUTION;
    s_pulse_count = 0;
    
    // پاک کردن بافر موقت برای اسکن جدید
    scan_process_clear_temp_buffer();

    scan_process_calculate_display_values();

    ESP_LOGI(TAG, "Scan process started and initialized");
}

void scan_process_stop(void)
{
    ESP_LOGI(TAG, "Scan process stopped. Recorded %d points in memory", s_temp_point_count);
}

void scan_process_handle_trigger(scan_mode_t mode)
{
    switch (mode) {
        case SCAN_MODE_MANPC:
        case SCAN_MODE_MANMEM:
            s_current_adc_value = rand() % (ADC_MAX_RESOLUTION + 1);
            s_pulse_count++;
            
            // اضافه کردن داده اسکن به بافر
            scan_process_add_point_to_buffer((int16_t)s_current_adc_value);
            
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
