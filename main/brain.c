#include "brain.h"
#include <stdio.h>
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"

static const char *TAG = "BRAIN";

// متغیرهای وضعیت
static app_page_t current_page = PAGE_SPLASH;
static int selected_menu = 0;
static bool menu_loaded = false;

// توابع خارجی تعریف شده در main.c یا بخش‌های دیگر برای مدیریت قفل LVGL
extern bool lvgl_lock(uint32_t timeout_ms);
extern void lvgl_unlock(void);

// تابع جدید مدیریت فوکوس منوی اصلی که در ui_MainMenu.c تعریف شده است
extern void menu_set_focused_index(int index);

// متغیر وضعیت اسپلش که در جای دیگر تعریف شده است
extern volatile bool splash_done;

void brain_init(void)
{
    current_page = PAGE_SPLASH;
    selected_menu = 0;
    menu_loaded = false;
    ESP_LOGI(TAG, "Brain initialized.");
}

app_page_t brain_get_current_page(void)
{
    return current_page;
}

int brain_get_selected_menu(void)
{
    return selected_menu;
}

bool brain_is_menu_loaded(void)
{
    return menu_loaded;
}

// تابع پردازش دستورات گرافیکی منتقل شده به brain.c
void brain_process_ui_cmds(void)
{
    // این تابع اکنون مستقیماً توسط تسک LVGL در main.c و درون حلقه قفل شده خوانده می‌شود.
    // بر اساس وضعیت صفحه جاری (current_page) اقدامات لازم صورت می‌گیرد.
    
    // نکته: ما منطق را بر اساس وضعیت‌های current_page هماهنگ می‌کنیم
    if (current_page == PAGE_MAIN_MENU && !menu_loaded)
    {
        if (splash_done)
        {
            menu_loaded = true;
            ui_MainMenu_screen_init();
            lv_screen_load(ui_MainMenu);
            
            // پاک‌سازی کامل صفحه اسپلش و آزاد کردن رم (بسیار مهم)
            ui_Screen1_cleanup_and_destroy();
            ESP_LOGI(TAG, "Main menu screen loaded & Splash destroyed");
        }
    }
}

// هاب اصلی تصمیم‌گیری بر اساس کلید
void brain_handle_key(key_evt_t evt)
{
    if (evt == KEY_NONE) return;

    ESP_LOGI(TAG, "Key: %d | Page: %d | Menu: %d", evt, current_page, selected_menu);

    switch (current_page)
    {
        case PAGE_SPLASH:
            if (evt == KEY_OK)
            {
                ESP_LOGI(TAG, "Splash OK -> Transitioning to Main Menu");
                current_page = PAGE_MAIN_MENU;
                selected_menu = 0;
            }
            break;

        case PAGE_MAIN_MENU:
            if (!menu_loaded) return; // اگر هنوز صفحه لود نشده کلیدها اثر نکنند

            // برای تغییرات گرافیکی LVGL از قفل استفاده می‌کنیم
            if (lvgl_lock(1000))
            {
                switch (evt)
                {
                    case KEY_UP:
                        selected_menu--;
                        if (selected_menu < 0) selected_menu = 4;
                        
                        // هدایت مستقیم فوکوس جدید به لایوت منو
                        menu_set_focused_index(selected_menu);
                        ESP_LOGI(TAG, "Menu Focus updated to: %d", selected_menu);
                        break;

                    case KEY_DOWN:
                        selected_menu++;
                        if (selected_menu > 4) selected_menu = 0;
                        
                        // هدایت مستقیم فوکوس جدید به لایوت منو
                        menu_set_focused_index(selected_menu);
                        ESP_LOGI(TAG, "Menu Focus updated to: %d", selected_menu);
                        break;

                    case KEY_OK:
                        ESP_LOGI(TAG, "Selected Menu Item %d Executed", selected_menu);
                        // در گام بعدی صفحات دیگر را لود خواهیم کرد
                        break;

                    case KEY_BACK:
                        ESP_LOGI(TAG, "Back pressed in Main Menu");
                        break;

                    default:
                        break;
                }
                lvgl_unlock();
            }
            break;

        default:
            break;
    }
}
