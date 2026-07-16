//brain.h
#pragma once

#include <stdbool.h>
#include "pcf8574.h"
#include "battery_process.h"

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
    SCAN_STATE_RUNNING
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

//int brain_get_scan_mode(void);



// Getter ها
app_page_t brain_get_current_page(void);
int brain_get_selected_menu(void);
bool brain_is_menu_loaded(void);
scan_mode_t brain_get_scan_mode(void);
battery_level_t brain_get_battery_level(void);
uint8_t brain_get_battery_percent(void);


