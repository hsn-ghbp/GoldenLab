#include "scan_process.h"
#include "brain.h"
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "SCAN_PROCESS";

// وضعیت‌های داخلی ماژول اسکن
typedef enum {
    SCAN_INTERNAL_STATE_IDLE = 0,
    SCAN_INTERNAL_STATE_WAIT_FOR_FIRST_TRIGGER,
    SCAN_INTERNAL_STATE_RUNNING
} scan_internal_state_t;

static scan_internal_state_t s_internal_state = SCAN_INTERNAL_STATE_IDLE;
static scan_mode_t s_current_mode = SCAN_MODE_MANPC;

// متغیرهای وضعیت سیگنال و مقادیر محاسباتی
static int s_current_adc_value = ADC_MID_RESOLUTION;
static int s_signed_value = 0;
static int s_positive_arc_value = 0;
static int s_negative_arc_value = 0;
static int s_needle_angle = 0;
static int s_pulse_count = 0;

// آفست کالیبراسیون و وضعیت آن
static int32_t s_calibration_offset = 0;
static bool s_is_calibrated = false;

// بافر موقت در RAM برای داده‌های اسکن جاری
static int16_t s_temp_scan_buffer[MAX_SCAN_POINTS];
static uint16_t s_temp_point_count = 0;

// تابع کمکی برای خواندن مستقیم از ADC سخت‌افزاری
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

// محاسبه مقادیر خروجی جهت نمایش در عقربه و نوارهای پیشرفت UI
static void scan_process_calculate_display_values(void)
{
    // اگر هنوز کالیبراسیون انجام نشده است، تمام مقادیر نمایش روی صفر تنظیم می‌شوند
    if (!s_is_calibrated) {
        s_signed_value = 0;
        s_positive_arc_value = 0;
        s_negative_arc_value = 0;
        s_needle_angle = 0;
        return;
    }

    int corrected_val = s_current_adc_value - s_calibration_offset;
    int diff = clamp_int(corrected_val, -ADC_MID_RESOLUTION, ADC_MID_RESOLUTION);
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
    srand(12345);
    scan_process_reset();
    ESP_LOGI(TAG, "Scan process initialized.");
}

void scan_process_reset(void)
{
    s_internal_state = SCAN_INTERNAL_STATE_IDLE;
    s_current_adc_value = ADC_MID_RESOLUTION;
    s_pulse_count = 0;
    s_calibration_offset = 0;
    s_is_calibrated = false;  // تا زمانی که تریگر زده نشود کالیبره نیست
    scan_process_clear_temp_buffer();
    scan_process_calculate_display_values(); // مقدار عقربه را صفر می‌کند
}


void scan_process_start(scan_mode_t mode)
{
    s_current_mode = mode;
    s_pulse_count = 0;
    scan_process_clear_temp_buffer();
    s_internal_state = SCAN_INTERNAL_STATE_WAIT_FOR_FIRST_TRIGGER;
    
    ESP_LOGI(TAG, "Scan started. Waiting for first trigger in Mode: %d", mode);
}

void scan_process_stop(void)
{
    s_internal_state = SCAN_INTERNAL_STATE_IDLE;
    ESP_LOGI(TAG, "Scan stopped. Total points collected: %d", s_temp_point_count);
}

bool scan_process_run_calibration(uint16_t sample_count)
{
    if (sample_count == 0) {
        s_calibration_offset = 0;
        s_is_calibrated = true;
        return true;
    }

    ESP_LOGI(TAG, "Starting auto-calibration with %d samples...", sample_count);
    int32_t sum = 0;
    for (uint16_t i = 0; i < sample_count; i++) {
        sum += read_hardware_adc();
    }

    s_calibration_offset = sum / sample_count;
    s_is_calibrated = true;
    
    ESP_LOGI(TAG, "Calibration completed. Offset: %ld", s_calibration_offset);
    return true;
}

// مدیریت وقوع تریگر با پذیرش پارامتر مود اسکن
void scan_process_handle_trigger(scan_mode_t mode)
{
    s_current_mode = mode;

    // اگر ماژول هنوز استارت نشده، آن را به عنوان اولین تریگر آغاز می‌کنیم
    if (s_internal_state == SCAN_INTERNAL_STATE_IDLE) {
        s_internal_state = SCAN_INTERNAL_STATE_WAIT_FOR_FIRST_TRIGGER;
    }

    // بررسی فشرده شدن تریگر اول
    if (s_internal_state == SCAN_INTERNAL_STATE_WAIT_FOR_FIRST_TRIGGER) {
        // ۱. انجام کالیبراسیون در صورت فعال بودن تنظیمات مربوطه
        if (g_settings.auto_cal) {
            scan_process_run_calibration(g_settings.auto_cal_pls);
        } else {
            s_calibration_offset = 0;
            s_is_calibrated = true;
        }

        // ۲. تفکیک منطق دستی و اتوماتیک
        if (s_current_mode == SCAN_MODE_MANPC || s_current_mode == SCAN_MODE_MANMEM) {
            // در حالت دستی: تریگر اول کالیبره می‌کند و آماده به کار می‌شود.
            s_internal_state = SCAN_INTERNAL_STATE_RUNNING;
            ESP_LOGI(TAG, "Manual Mode: Calibrated on 1st trigger. Ready for next pulses.");
            return; // خارج می‌شود و ثبت نمونه به تریگرهای بعدی موکول می‌شود
        } else {
            // در حالت اتوماتیک: بلافاصله پس از کالیبره وارد حالت ثبت داده و نمونه اول می‌شود.
            s_internal_state = SCAN_INTERNAL_STATE_RUNNING;
            ESP_LOGI(TAG, "Auto Mode: Calibrated on 1st trigger. Auto sampling starts now.");
        }
    }

    // ثبت نمونه در وضعیت فعال (Running)
    if (s_internal_state == SCAN_INTERNAL_STATE_RUNNING) {
        s_current_adc_value = read_hardware_adc();
        int16_t corrected = (int16_t)(s_current_adc_value - s_calibration_offset);
        
        if (scan_process_add_point_to_buffer(corrected)) {
            s_pulse_count++;
        }
        
        scan_process_calculate_display_values();
        ESP_LOGI(TAG, "Pulse recorded: #%d, Raw ADC: %d, Offset-Corrected: %d", 
                 s_pulse_count, s_current_adc_value, corrected);
    }
}

/* Getterها */
int scan_process_get_current_adc_value(void) { return s_current_adc_value; }
int scan_process_get_signed_value(void) { return s_signed_value; }
int scan_process_get_positive_arc_value(void) { return s_positive_arc_value; }
int scan_process_get_negative_arc_value(void) { return s_negative_arc_value; }
int scan_process_get_needle_angle(void) { return s_needle_angle; }
int scan_process_get_pulse_count(void) { return s_pulse_count; }
bool scan_process_is_calibrated(void) { return s_is_calibrated; }

scan_sub_state_t scan_process_get_sub_state(void)
{
    if (s_internal_state == SCAN_INTERNAL_STATE_RUNNING) {
        return SCAN_STATE_RUNNING;
    }
    return SCAN_STATE_IDLE;
}
