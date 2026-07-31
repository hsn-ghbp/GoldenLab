//brain.h
#pragma once

#include <stdbool.h>
#include "pcf8574.h"
#include "battery_process.h"
#include <stdint.h>

typedef enum {
    PAGE_NONE = 0,
    PAGE_SPLASH,
    PAGE_MAIN_MENU,
    PAGE_SCAN,
    PAGE_SCAN_PAGE,
    PAGE_SEND,
    PAGE_MEMORY,
    PAGE_SETTING,
    PAGE_ABOUT
} app_page_t;




typedef enum {
    SCAN_STATE_IDLE = 0,
    SCAN_STATE_RUNNING,
    SCAN_STATE_STOPPED_WAIT_BACK
} scan_sub_state_t;

typedef enum {
    APP_EVENT_NONE         = 0,
    APP_EVENT_SCAN_CHANGED = (1 << 0),
    APP_EVENT_BATTERY_CHANGED = (1 << 1)
} app_event_t;

typedef enum {
    SCAN_MODE_MANPC = 0,
    SCAN_MODE_AUTOPC = 1,
    SCAN_MODE_AUTOMEM = 2,
    SCAN_MODE_MANMEM = 3
} scan_mode_t;

typedef enum {
    SETTING_STATE_LIST = 0,
    SETTING_STATE_OPENING,
    SETTING_STATE_DETAIL,
    SETTING_STATE_CLOSING,
} setting_state_t;


// تعاریف اندیس‌های منو برای خوانایی بیشتر کد
typedef enum {
    SETTING_ITEM_AUTOCAL = 0,      // bool
    SETTING_ITEM_AUTOCAL_PLS,      // int
    SETTING_ITEM_PULS_MAX,         // int
    SETTING_ITEM_DELAY_TIME,       // int
    SETTING_ITEM_STOP_TRG,         // bool
    SETTING_ITEM_BEEP,             // bool
    SETTING_ITEM_BL_AUTO_OFF,      // bool
    SETTING_ITEM_BL_AUTO_CONNECT,  // bool
    SETTING_ITEM_BL_PASS,          // int
    SETTING_ITEM_BL_NAME           // string (char array)
} setting_item_index_t;

#define SETTING_ITEM_COUNT 10

// ساختار متمرکز تنظیمات MAGI_ESP
typedef struct {
    bool auto_cal;           // setting_items[0]
    int32_t auto_cal_pls;    // setting_items[1]
    int32_t puls_max;        // setting_items[2]
    int32_t delay_time;      // setting_items[3]
    bool stop_trg;           // setting_items[4]
    bool beep;               // setting_items[5]
    bool bl_auto_off;        // setting_items[6]
    bool bl_auto_connect;    // setting_items[7]
    int32_t bl_pass;         // setting_items[8]
    char bl_name[32];        // setting_items[9]
} system_settings_t;

extern system_settings_t g_settings;

// توابع مدیریت و دسترسی به تنظیمات از سمت Brain
const system_settings_t* brain_get_settings(void);

// تابع متمرکز تغییر مقادیر بولین با استفاده از اندیس
void brain_set_bool_setting(setting_item_index_t index, bool value);
void brain_toggle_bool_setting(setting_item_index_t index);
void brain_setting_detail_step(bool increase);
scan_sub_state_t brain_get_scan_sub_state(void);
void brain_emit_event(app_event_t event);
app_event_t brain_consume_events(void);

// توابع اصلی Brain
void brain_init(void);
void brain_handle_key(key_evt_t evt);
void brain_process_ui_cmds(void);
int brain_get_scan_selected(void);
battery_level_t brain_get_battery_level(void);
uint8_t brain_get_battery_percent(void);
bool brain_should_bluetooth_be_enabled(void);
/* اختیاری ولی مفید */
bool brain_settings_load(void);
bool brain_settings_save(void);
void brain_settings_set_defaults(void);

//int brain_get_scan_mode(void);



// Getter ها
app_page_t brain_get_current_page(void);
int brain_get_selected_menu(void);
bool brain_is_menu_loaded(void);
scan_mode_t brain_get_scan_mode(void);
battery_level_t brain_get_battery_level(void);
uint8_t brain_get_battery_percent(void);


/* Setting state accessors */
int brain_get_setting_index(void);
void brain_set_setting_index(int index);
void brain_setting_next(void);
void brain_setting_prev(void);



