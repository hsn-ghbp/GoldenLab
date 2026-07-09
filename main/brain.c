#include "brain.h"
#include <stdbool.h>
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"
#include "ui_MainMenu.h"

static const char *TAG = "BRAIN";

// -------------------------
// App state
// -------------------------
static app_page_t current_page = PAGE_SPLASH;
static app_page_t loaded_page  = PAGE_SPLASH;

static int selected_menu = 0;
static int scan_selected = 0;

static int last_applied_menu_focus = -1;
static int last_applied_scan_focus = -1;

// -------------------------
// External symbols
// -------------------------
extern volatile bool splash_done;

extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_MainMenu;
extern lv_obj_t *ui_ScanMenu;

extern void menu_set_focused_index(int index);
extern void scan_set_focused_index(int index);

extern void ui_Screen1_cleanup_and_destroy(void);
extern void ui_MainMenu_screen_init(void);
extern void ui_MainMenu_screen_destroy(void);
extern void ui_ScanMenu_screen_init(void);
extern void ui_ScanMenu_screen_destroy(void);

// -------------------------
// Internal helpers
// -------------------------
static void brain_destroy_page(app_page_t page)
{
    switch (page)
    {
        case PAGE_SPLASH:
            if (ui_Screen1) {
                ui_Screen1_cleanup_and_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SPLASH");
            }
            break;

        case PAGE_MAIN_MENU:
            if (ui_MainMenu) {
                ui_MainMenu_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_MAIN_MENU");
            }
            break;

        case PAGE_SCAN:
            if (ui_ScanMenu) {
                ui_ScanMenu_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SCAN");
            }
            break;

        default:
            // برای صفحه‌هایی که هنوز destroy ندارند فعلاً کاری نکن
            break;
    }
}

static bool brain_prepare_page(app_page_t page, lv_obj_t **out_screen)
{
    if (out_screen == NULL) {
        ESP_LOGE(TAG, "brain_prepare_page: out_screen is NULL");
        return false;
    }

    *out_screen = NULL;

    switch (page)
    {
        case PAGE_MAIN_MENU:
            ui_MainMenu_screen_init();
            if (ui_MainMenu == NULL) {
                ESP_LOGE(TAG, "Failed to prepare PAGE_MAIN_MENU");
                return false;
            }
            *out_screen = ui_MainMenu;
            ESP_LOGI(TAG, "Prepared PAGE_MAIN_MENU");
            return true;

        case PAGE_SCAN:
            ui_ScanMenu_screen_init();
            if (ui_ScanMenu == NULL) {
                ESP_LOGE(TAG, "Failed to prepare PAGE_SCAN");
                return false;
            }
            *out_screen = ui_ScanMenu;
            ESP_LOGI(TAG, "Prepared PAGE_SCAN");
            return true;

        default:
            ESP_LOGW(TAG, "Page %d is not implemented or not supported yet", page);
            return false;
    }
}

static bool brain_transition_to_page(app_page_t target_page)
{
    lv_obj_t *new_screen = NULL;
    app_page_t previous_page = loaded_page;

    if (target_page == loaded_page) {
        return true;
    }

    if (!brain_prepare_page(target_page, &new_screen)) {
        ESP_LOGW(TAG, "Transition rejected: prepare failed for page %d", target_page);
        return false;
    }

    if (new_screen == NULL) {
        ESP_LOGE(TAG, "Transition rejected: new_screen is NULL for page %d", target_page);
        return false;
    }

    // اول صفحه جدید را لود می‌کنیم
    lv_screen_load(new_screen);
    ESP_LOGI(TAG, "Loaded page %d", target_page);

    // بعد صفحه قبلی را destroy می‌کنیم تا صفحه خالی نشود
    brain_destroy_page(previous_page);

    loaded_page = target_page;

    // ریست فوکوس cache برای اعمال مجدد روی صفحه جدید
    if (target_page == PAGE_MAIN_MENU) {
        last_applied_menu_focus = -1;
    } else if (target_page == PAGE_SCAN) {
        last_applied_scan_focus = -1;
    }

    return true;
}

static void brain_apply_focus_if_needed(void)
{
    switch (loaded_page)
    {
        case PAGE_MAIN_MENU:
            if (selected_menu != last_applied_menu_focus) {
                menu_set_focused_index(selected_menu);
                last_applied_menu_focus = selected_menu;
                ESP_LOGD(TAG, "Applied main menu focus: %d", selected_menu);
            }
            break;

        case PAGE_SCAN:
            if (scan_selected != last_applied_scan_focus) {
                scan_set_focused_index(scan_selected);
                last_applied_scan_focus = scan_selected;
                ESP_LOGD(TAG, "Applied scan focus: %d", scan_selected);
            }
            break;

        default:
            break;
    }
}

// -------------------------
// Public API
// -------------------------
void brain_init(void)
{
    current_page = PAGE_SPLASH;
    loaded_page  = PAGE_SPLASH;

    selected_menu = 0;
    scan_selected = 0;

    last_applied_menu_focus = -1;
    last_applied_scan_focus = -1;

    ESP_LOGI(TAG, "Brain initialized");
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
    return (loaded_page == PAGE_MAIN_MENU);
}

void brain_handle_key(key_evt_t evt)
{
    if (evt == KEY_NONE) {
        return;
    }

    ESP_LOGI(TAG, "Key event: %d, current_page: %d", evt, current_page);

    switch (current_page)
    {
        case PAGE_SPLASH:
            if ((evt == KEY_OK) && splash_done) {
                current_page = PAGE_MAIN_MENU;
                selected_menu = 0;
            }
            break;

        case PAGE_MAIN_MENU:
            if (evt == KEY_UP) {
                selected_menu = (selected_menu - 1 + 5) % 5;
            }
            else if (evt == KEY_DOWN) {
                selected_menu = (selected_menu + 1) % 5;
            }
            else if (evt == KEY_OK) {
                switch (selected_menu)
                {
                    case 0:
                        current_page = PAGE_SCAN;
                        scan_selected = 0;
                        break;

                    case 1:
                        current_page = PAGE_SEND;
                        break;

                    case 2:
                        current_page = PAGE_MEMORY;
                        break;

                    case 3:
                        current_page = PAGE_SETTING;
                        break;

                    case 4:
                        current_page = PAGE_ABOUT;
                        break;

                    default:
                        break;
                }
            }
            break;

        case PAGE_SCAN:
            if (evt == KEY_BACK) {
                current_page = PAGE_MAIN_MENU;
            }
            else if (evt == KEY_UP) {
                scan_selected = (scan_selected - 1 + 4) % 4;
            }
            else if (evt == KEY_DOWN) {
                scan_selected = (scan_selected + 1) % 4;
            }
            else if (evt == KEY_OK) {
                ESP_LOGI(TAG, "Scan item selected: %d", scan_selected);
            }
            break;

        case PAGE_SEND:
        case PAGE_MEMORY:
        case PAGE_SETTING:
        case PAGE_ABOUT:
            if (evt == KEY_BACK) {
                current_page = PAGE_MAIN_MENU;
            }
            break;

        default:
            break;
    }
}

// void brain_process_ui_cmds(void)
// {
//     if (current_page != loaded_page) {
//         if (!brain_transition_to_page(current_page)) {
//             ESP_LOGW(TAG, "Failed to transition to page %d, reverting to %d", current_page, loaded_page);
//             current_page = loaded_page;
//         }
//     }

//     brain_apply_focus_if_needed();
// }

void brain_process_ui_cmds(void)
{
    static app_page_t last_logged_current = -1;
    static app_page_t last_logged_loaded = -1;

    if (current_page != last_logged_current || loaded_page != last_logged_loaded) {
        ESP_LOGW(TAG, "STATE current=%d loaded=%d splash_done=%d",
                 current_page, loaded_page, splash_done);
        last_logged_current = current_page;
        last_logged_loaded = loaded_page;
    }

    if (current_page != loaded_page) {
        ESP_LOGW(TAG, "TRANSITION requested: %d -> %d", loaded_page, current_page);

        if (!brain_transition_to_page(current_page)) {
            ESP_LOGW(TAG, "Failed to transition to page %d, reverting to %d",
                     current_page, loaded_page);
            current_page = loaded_page;
        }
    }

    brain_apply_focus_if_needed();
}

