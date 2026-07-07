#pragma once

#include "pcf8574.h" // برای دسترسی به key_evt_t

// تعریف صفحات مختلف برنامه
typedef enum {
    PAGE_SPLASH = 0,
    PAGE_MAIN_MENU,
    PAGE_ABOUT,
    PAGE_SETTING,
    PAGE_MEMORY,
    PAGE_SEND,
    PAGE_SCAN
} app_page_t;

// توابع اصلی کنترلر
void brain_init(void);
void brain_handle_key(key_evt_t evt);
app_page_t brain_get_current_page(void);
int brain_get_selected_menu(void);
