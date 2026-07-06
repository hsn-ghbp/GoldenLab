#ifndef UI_SHARED_DEFS_H
#define UI_SHARED_DEFS_H

// تعریف حالت‌های مختلف برنامه برای کنترل رفتار کلیدها
typedef enum {
    APP_STATE_SPLASH,
    APP_STATE_MENU,
    APP_STATE_SCAN,
    APP_STATE_SEND,
    APP_STATE_MEMORY,
    APP_STATE_SETTINGS,
    APP_STATE_INFO
} app_state_t;

// کدهای عملیاتی فرستاده شده به صف UI
typedef enum {
    UI_CMD_NONE = 0,
    UI_CMD_LOAD_MENU,
    
    // دستورات ناوبری منوی اصلی
    UI_CMD_MENU_NEXT,
    UI_CMD_MENU_PREV,
    
    // باز کردن زیرمنوها
    UI_CMD_OPEN_SCAN,
    UI_CMD_OPEN_SEND,
    UI_CMD_OPEN_MEMORY,
    UI_CMD_OPEN_SETTINGS,
    UI_CMD_OPEN_INFO,
    
    // بازگشت به منوی اصلی
    UI_CMD_BACK_TO_MENU
} ui_cmd_t;

#endif // UI_SHARED_DEFS_H
