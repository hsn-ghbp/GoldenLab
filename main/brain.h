#pragma once

#include "pcf8574.h"

typedef enum {
    PAGE_SPLASH = 0,
    PAGE_MAIN_MENU,
    PAGE_ABOUT,
    PAGE_SETTING,
    PAGE_MEMORY,
    PAGE_SEND,
    PAGE_SCAN
} app_page_t;

// توابع اصلی
void brain_init(void);
void brain_handle_key(key_evt_t evt);
void brain_process_ui_cmds(void); // تابع جدید منتقل شده

app_page_t brain_get_current_page(void);
int brain_get_selected_menu(void);
bool brain_is_menu_loaded(void);
