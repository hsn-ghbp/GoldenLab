#include "battery_process.h"

#include "esp_log.h"
#include "driver/adc.h"
#include "brain.h"

static const char *TAG = "battery_process";

#define BATTERY_MIN_MV      3300
#define BATTERY_MAX_MV      4200

#define BATTERY_ADC_CHANNEL ADC1_CHANNEL_0   // GPIO36
#define BATTERY_ADC_ATTEN   ADC_ATTEN_DB_11
#define BATTERY_ADC_WIDTH   ADC_WIDTH_BIT_12

static uint16_t s_battery_adc_value = 0;
static uint16_t s_battery_voltage_mv = 0;
static uint8_t s_battery_percent = 0;
static battery_level_t s_battery_level = BATTERY_LEVEL_EMPTY;
static battery_level_t s_prev_battery_level = BATTERY_LEVEL_EMPTY;
static battery_level_t current_level = BATTERY_LEVEL_EMPTY;

static uint16_t battery_adc_read_raw(void)
{
    int raw = adc1_get_raw(BATTERY_ADC_CHANNEL);

    if (raw < 0) {
        raw = 0;
    }

    return (uint16_t)raw;
}

static uint16_t battery_process_read_adc(void)
{
    return battery_adc_read_raw();
}

static uint16_t battery_process_adc_to_mv(uint16_t adc_value)
{
    // فرض: 12-bit ADC و ورودی ADC حدود 3.3V و تقسیم مقاومتی 1/2
    // اگر مقاومت‌ها متفاوت‌اند، این فرمول را اصلاح کن
    uint32_t adc_input_mv = (uint32_t)adc_value * 3300 / 4095;
    return (uint16_t)(adc_input_mv * 2);
}

static uint8_t battery_process_voltage_to_percent(uint16_t voltage_mv)
{
    if (voltage_mv <= BATTERY_MIN_MV) {
        return 0;
    }

    if (voltage_mv >= BATTERY_MAX_MV) {
        return 100;
    }

    return (uint32_t)(voltage_mv - BATTERY_MIN_MV) * 100 /
           (BATTERY_MAX_MV - BATTERY_MIN_MV);
}

static battery_level_t battery_process_percent_to_level(uint8_t percent)
{
    if (percent <= 10) {
        return BATTERY_LEVEL_EMPTY;
    } else if (percent <= 35) {
        return BATTERY_LEVEL_25;
    } else if (percent <= 60) {
        return BATTERY_LEVEL_50;
    } else if (percent <= 85) {
        return BATTERY_LEVEL_75;
    }

    return BATTERY_LEVEL_FULL;
}

void battery_process_init(void)
{
    s_battery_adc_value = 0;
    s_battery_voltage_mv = 0;
    s_battery_percent = 0;
    s_battery_level = BATTERY_LEVEL_EMPTY;
    s_prev_battery_level = BATTERY_LEVEL_EMPTY;

    adc1_config_width(BATTERY_ADC_WIDTH);
    adc1_config_channel_atten(BATTERY_ADC_CHANNEL, BATTERY_ADC_ATTEN);

    ESP_LOGI(TAG, "Battery process initialized");
}

void battery_process_update(void)
{
    battery_level_t old_level = s_battery_level;

    s_battery_adc_value = battery_process_read_adc();
    s_battery_voltage_mv = battery_process_adc_to_mv(s_battery_adc_value);
    s_battery_percent = battery_process_voltage_to_percent(s_battery_voltage_mv);
    s_battery_level = battery_process_percent_to_level(s_battery_percent);

    ESP_LOGI(TAG, "Battery adc=%u voltage=%umV percent=%u level=%d",
             s_battery_adc_value,
             s_battery_voltage_mv,
             s_battery_percent,
             s_battery_level);

    // اگر سطح باتری نسبت به قبل تغییر کرده بود، رویداد صادر می‌شود
    battery_level_t new_level =s_battery_level;
    if (new_level != current_level) {
        current_level = new_level;
        ESP_LOGI(TAG, "Battery level changed! Emitting Event...");
        brain_emit_event(APP_EVENT_BATTERY_CHANGED);
    }
}


uint16_t battery_process_get_adc_value(void)
{
    return s_battery_adc_value;
}

uint16_t battery_process_get_voltage_mv(void)
{
    return s_battery_voltage_mv;
}

uint8_t battery_process_get_percent(void)
{
    return s_battery_percent;
}

battery_level_t battery_process_get_level(void)
{
    return current_level;
}
