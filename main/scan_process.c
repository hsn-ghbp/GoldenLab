#include "scan_process.h"
#include "brain.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bluetooth.h"

static const char *TAG = "SCAN_PROCESS";

static int s_current_mode = 0;
static bool s_running = false;
static bool s_is_calibrated = false;
static volatile bool s_stop_requested = false;
static TaskHandle_t s_multi_task_handle = NULL;

static int s_current_adc_value = ADC_MID_RESOLUTION;
static int s_signed_value = 0;
static int s_positive_arc_value = 0;
static int s_negative_arc_value = 0;
static int s_needle_angle = 0;
static int s_pulse_count = 0;
static uint32_t s_calibration_sample_count = 0U;

static int16_t s_temp_scan_buffer[MAX_SCAN_POINTS];
static uint16_t s_temp_point_count = 0;
static bool s_rand_seeded = false;


static int s_multi_delay_ms = 0;
uint16_t actual_count = 0;


uint32_t scan_process_get_calibration_sample_count(void)
{
    return s_calibration_sample_count;
}

static int16_t read_hardware_adc(void)
{
    // در پروژه واقعی: return adc_read_raw_value();
    return (int16_t)(ADC_MID_RESOLUTION + (rand() % 400 - 200));
}

static int clamp_int(int value, int min_value, int max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
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
    int magnitude = (diff >= 0) ? diff : -diff;

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

static bool scan_mode_is_memory(int mode)
{
    return (mode == SCAN_MODE_AUTOMEM || mode == SCAN_MODE_MANMEM);
}

static bool scan_mode_is_pc_send(int mode)
{
    return (mode == SCAN_MODE_AUTOPC || mode == SCAN_MODE_MANPC);
}

static void scan_process_send_bt_placeholder(int16_t value)
{
    if (bluetooth_is_enabled()) {
        bluetooth_send_int32((int32_t)value);
        ESP_LOGD(TAG, "Sent to BT: %d", value);
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
        ESP_LOGW(TAG, "Scan buffer full");
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

    s_current_mode = SCAN_MODE_MANPC;
    s_running = false;
    s_is_calibrated = false;
    s_stop_requested = false;
    s_multi_task_handle = NULL;

    s_current_adc_value = ADC_MID_RESOLUTION;
    s_signed_value = 0;
    s_positive_arc_value = 0;
    s_negative_arc_value = 0;
    s_needle_angle = 0;
    s_pulse_count = 0;

    scan_process_clear_temp_buffer();
    scan_process_calculate_display_values();

    ESP_LOGI(TAG, "Scan process initialized");
}

void scan_process_start(int mode)
{
    s_current_mode = mode;
    s_running = true;
    s_is_calibrated = false;
    s_stop_requested = false;
    s_pulse_count = 0;

    s_current_adc_value = ADC_MID_RESOLUTION;
    s_signed_value = 0;
    s_positive_arc_value = 0;
    s_negative_arc_value = 0;
    s_needle_angle = 0;
    s_calibration_sample_count = 0U;

    scan_process_clear_temp_buffer();
    scan_process_calculate_display_values();

    ESP_LOGI(TAG, "Scan process started. mode=%d", mode);
}

void scan_process_stop(void)
{
    s_stop_requested = true;
    s_running = false;
    ESP_LOGI(TAG, "Scan process stop requested");
}

// static bool scan_process_send_calibration_block_from_buffer(const int16_t *data, uint16_t count)
// {
//     if (data == NULL || count == 0) {
//         ESP_LOGW(TAG, "Calibration send skipped: empty buffer");
//         return false;
//     }

//     if (!bluetooth_is_enabled() || !bluetooth_is_connected()) {
//         ESP_LOGW(TAG, "Calibration send skipped: bluetooth not connected");
//         return false;
//     }

//     char tx_buffer[256];
//     int offset = 0;

//     for (uint16_t i = 0; i < count; i++) {
//         int written = snprintf(
//             tx_buffer + offset,
//             sizeof(tx_buffer) - offset,
//             (i == 0) ? "%d" : ",%d",
//             data[i]
//         );

//         if (written < 0 || written >= (int)(sizeof(tx_buffer) - offset)) {
//             ESP_LOGE(TAG, "Calibration tx buffer overflow at index=%u", i);
//             return false;
//         }

//         offset += written;
//     }

//     if (offset + 2 >= (int)sizeof(tx_buffer)) {
//         ESP_LOGE(TAG, "Calibration tx buffer has no room for line ending");
//         return false;
//     }

//     tx_buffer[offset++] = '\r';
//     tx_buffer[offset++] = '\n';

//     esp_err_t err = bluetooth_send_raw((const uint8_t *)tx_buffer, offset);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Calibration block send failed: %s", esp_err_to_name(err));
//         return false;
//     }

//     ESP_LOGI(TAG, "Calibration block sent. count=%u, bytes=%d", count, offset);
//     return true;
// }



bool scan_process_calibrate_now(void)
{
    uint16_t sample_count = g_settings.auto_cal_pls;

    if (sample_count == 0) {
        s_is_calibrated = true;
        s_calibration_sample_count = 0;
        ESP_LOGI(TAG, "Calibration skipped because auto_cal_pls=0");
        return true;
    }

    ESP_LOGI(TAG, "Calibration started. samples=%u mode=%d", sample_count, s_current_mode);

    uint16_t actual_count = 0;

    if (scan_mode_is_pc_send(s_current_mode)) {
        static int32_t bt_samples[1024];

        if (sample_count > (sizeof(bt_samples) / sizeof(bt_samples[0]))) {
            ESP_LOGW(TAG, "Calibration sample_count=%u exceeds bt_samples capacity=%u",
                     sample_count,
                     (unsigned)(sizeof(bt_samples) / sizeof(bt_samples[0])));
            return false;
        }

        for (uint16_t i = 0; i < sample_count; i++) {
            int16_t raw = read_hardware_adc();
            s_current_adc_value = raw;
            scan_process_calculate_display_values();
            bt_samples[i] = (int32_t)raw;
            actual_count++;
        }

        esp_err_t err = bluetooth_send_int32_stream_begin(0, bt_samples, actual_count);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Calibration send array failed: %s", esp_err_to_name(err));
            return false;
        }
    } else {
        for (uint16_t i = 0; i < sample_count; i++) {
            int16_t raw = read_hardware_adc();
            s_current_adc_value = raw;
            scan_process_calculate_display_values();

            if (scan_mode_is_memory(s_current_mode)) {
                if (!scan_process_add_point_to_buffer(raw)) {
                    ESP_LOGW(TAG, "Calibration buffer full at sample %u", i);
                    break;
                }
                actual_count++;
            }
        }
    }

    s_is_calibrated = true;
    s_calibration_sample_count = actual_count;
    ESP_LOGI(TAG, "Calibration finished. actual=%u", actual_count);
    return true;
}






// bool scan_process_calibrate_now(void)
// {
//     uint16_t sample_count = g_settings.auto_cal_pls;

//     if (sample_count == 0) {
//         s_is_calibrated = true;
//         ESP_LOGI(TAG, "Calibration skipped because auto_cal_pls=0");
//         return true;
//     }

//     ESP_LOGI(TAG, "Calibration started. samples=%u mode=%d", sample_count, s_current_mode);

//     for (uint16_t i = 0; i < sample_count; i++) {
//         int16_t raw = read_hardware_adc();
//         s_current_adc_value = raw;
//         scan_process_calculate_display_values();

//         if (scan_mode_is_memory(s_current_mode)) {
//             if (!scan_process_add_point_to_buffer(raw)) {
//                 ESP_LOGW(TAG, "Calibration buffer full at sample %u", i);
//                 break;
//             }
//         } 
//         else if (scan_mode_is_pc_send(s_current_mode)) {
//             esp_err_t err = bluetooth_send_int32((int32_t)raw);
//             if (err != ESP_OK) {
//                 ESP_LOGW(TAG, "Calibration enqueue failed at sample %u: %s", i, esp_err_to_name(err));
//             }
//         }
//     }

//     s_is_calibrated = true;
//     ESP_LOGI(TAG, "Calibration finished. Samples queued individually.");
//     return true;
// }




  

bool scan_process_capture_one_pulse(void)
{
    if (!s_running) {
        ESP_LOGW(TAG, "capture_one rejected: process not running");
        return false;
    }

    int16_t raw = read_hardware_adc();
    s_current_adc_value = raw;

    if (scan_mode_is_memory(s_current_mode)) {
    if (!scan_process_add_point_to_buffer(raw)) {
        ESP_LOGW(TAG, "capture_one failed: buffer full");
        return false;
    }
    } else if (scan_mode_is_pc_send(s_current_mode)) {
        scan_process_send_bt_placeholder(raw);
    }


    s_pulse_count++;
    scan_process_calculate_display_values();

    ESP_LOGI(TAG, "capture_one raw=%d pulse=%d", raw, s_pulse_count);
    return true;
}

static void scan_process_multi_task(void *arg)
{
    (void)arg;

    ESP_LOGI(TAG, "Auto multi task started. delay=%d ms", s_multi_delay_ms);

    while (!s_stop_requested) {
        if (!scan_process_capture_one_pulse()) {
            ESP_LOGW(TAG, "Auto multi task stopped because capture failed");
            break;
        }

        brain_emit_event(APP_EVENT_SCAN_CHANGED);

        if (s_multi_delay_ms > 0) {
            vTaskDelay(pdMS_TO_TICKS(s_multi_delay_ms));
        } else {
            taskYIELD();
        }
    }

    s_multi_task_handle = NULL;
    ESP_LOGI(TAG, "Auto multi task exited. pulse_count=%d", s_pulse_count);
    vTaskDelete(NULL);
}

bool scan_process_capture_multi_pulse(int delay_ms)
{
    if (!s_running) {
        ESP_LOGW(TAG, "capture_multi rejected: process not running");
        return false;
    }

    if (s_multi_task_handle != NULL) {
        ESP_LOGW(TAG, "capture_multi rejected: task already running");
        return false;
    }

    s_stop_requested = false;
    s_multi_delay_ms = (delay_ms < 0) ? 0 : delay_ms;

    BaseType_t ok = xTaskCreate(
        scan_process_multi_task,
        "scan_multi_task",
        4096,
        NULL,
        5,
        &s_multi_task_handle
    );

    if (ok != pdPASS) {
        s_multi_task_handle = NULL;
        ESP_LOGE(TAG, "Failed to create auto multi task");
        return false;
    }

    ESP_LOGI(TAG, "Auto multi task creation success");
    return true;
}

bool scan_process_is_running(void)
{
    return s_running;
}

bool scan_process_is_calibrated(void)
{
    return s_is_calibrated;
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
