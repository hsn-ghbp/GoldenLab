#pragma once

#include <stdbool.h>
#include "pcf8574.h"

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

// توابع اصلی Brain
void brain_init(void);
void brain_handle_key(key_evt_t evt);
void brain_process_ui_cmds(void);

// Getter ها
app_page_t brain_get_current_page(void);
int brain_get_selected_menu(void);
bool brain_is_menu_loaded(void);
