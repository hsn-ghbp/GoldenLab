#include "brain.h"
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "BRAIN";

// متغیرهای وضعیت (State)
static app_page_t current_page = PAGE_SPLASH;
static int selected_menu = 0;

void brain_init(void)
{
    current_page = PAGE_SPLASH;
    selected_menu = 0;
    ESP_LOGI(TAG, "Brain initialized. Current page: SPLASH");
}

app_page_t brain_get_current_page(void)
{
    return current_page;
}

int brain_get_selected_menu(void)
{
    return selected_menu;
}

// هاب اصلی تصمیم‌گیری بر اساس کلید فشرده شده و صفحه جاری
void brain_handle_key(key_evt_t evt)
{
    if (evt == KEY_NONE) return;

    ESP_LOGI(TAG, "Key Event Received: %d | Page: %d | Menu Index: %d", evt, current_page, selected_menu);

    switch (current_page)
    {
        case PAGE_SPLASH:
            if (evt == KEY_OK)
            {
                ESP_LOGI(TAG, "[Action] Splash -> Main Menu");
                current_page = PAGE_MAIN_MENU;
                selected_menu = 0;
            }
            else
            {
                ESP_LOGI(TAG, "[Action] Ignore key %d in Splash Screen (Only OK allowed)", evt);
            }
            break;

        case PAGE_MAIN_MENU:
            switch (evt)
            {
                case KEY_UP:
                    selected_menu--;
                    if (selected_menu < 0) selected_menu = 4; // فرض بر ۵ آیتم منو
                    ESP_LOGI(TAG, "[Action] Main Menu UP -> Index: %d", selected_menu);
                    break;

                case KEY_DOWN:
                    selected_menu++;
                    if (selected_menu > 4) selected_menu = 0;
                    ESP_LOGI(TAG, "[Action] Main Menu DOWN -> Index: %d", selected_menu);
                    break;

                case KEY_OK:
                    ESP_LOGI(TAG, "[Action] Main Menu OK -> Load sub-page for index: %d", selected_menu);
                    // اینجا بعداً بر اساس selected_menu صفحه را به PAGE_ABOUT، PAGE_SETTING و غیره تغییر می‌دهیم.
                    break;

                case KEY_BACK:
                    ESP_LOGI(TAG, "[Action] Main Menu BACK -> Return to Splash");
                    current_page = PAGE_SPLASH;
                    break;

                default:
                    break;
            }
            break;

        case PAGE_ABOUT:
        case PAGE_SETTING:
        case PAGE_MEMORY:
        case PAGE_SEND:
        case PAGE_SCAN:
            if (evt == KEY_BACK)
            {
                ESP_LOGI(TAG, "[Action] Sub-page -> Back to Main Menu");
                current_page = PAGE_MAIN_MENU;
            }
            else
            {
                ESP_LOGI(TAG, "[Action] Sub-page Key Pressed: %d", evt);
            }
            break;

        default:
            break;
    }
}
