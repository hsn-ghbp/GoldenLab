#ifndef SCAN_PROCESS_H
#define SCAN_PROCESS_H

#include "brain.h"
#include <stdint.h>
#include <stdbool.h>

#define ADC_MAX_RESOLUTION 4095
#define ADC_MID_RESOLUTION (ADC_MAX_RESOLUTION / 2)
#define SCAN_NEEDLE_MAX_ANGLE 900
#define MAX_SCAN_POINTS 500  // حداکثر نقاط قابل ذخیره در یک اسکن

// ساختار اطلاعات خلاصه برای ایندکس حافظه
typedef struct {
    uint32_t id;
    uint8_t mode;
    uint16_t point_count;
    char timestamp[20]; // فرمت YYYY-MM-DD HH:MM
} scan_index_item_t;

// ساختار کامل اطلاعات اسکن برای فایل دیتا
typedef struct {
    uint32_t id;
    uint8_t mode;
    uint16_t point_count;
    int16_t adc_data[MAX_SCAN_POINTS];
    char timestamp[20];
} scan_record_t;

void scan_process_init(void);
void scan_process_stop(void);
void scan_process_handle_trigger(scan_mode_t mode);

int scan_process_get_current_adc_value(void);
int scan_process_get_signed_value(void);
int scan_process_get_positive_arc_value(void);
int scan_process_get_negative_arc_value(void);
int scan_process_get_needle_angle(void);
int scan_process_get_pulse_count(void);

// توابع جدید برای ذخیره داده‌های موقت اسکن فعال
void scan_process_clear_temp_buffer(void);
bool scan_process_add_point_to_buffer(int16_t adc_val);
const int16_t* scan_process_get_buffer_data(uint16_t *out_count);

#endif // SCAN_PROCESS_H
