//brain.c
#include "brain.h"
#include <stdbool.h>
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"

#include "ui_MainMenu.h"
#include "ui_ScanMenu.h"
#include "ui_Screen1.h"
#include "ui_About.h"
#include "ui_Memory.h"
#include "ui_SendData.h"
#include "ui_Setting.h"
#include "ui_ScanPage.h"
#include "scan_process.h"
#include "battery_process.h"
#include "app_settings.h"
//#include "bluetooth_mgr.h"


static const char *TAG = "BRAIN";

// -------------------------
// App state
// -------------------------
static app_page_t current_page = PAGE_SPLASH;
static app_page_t loaded_page  = PAGE_SPLASH;
static scan_sub_state_t current_scan_sub_state = SCAN_STATE_IDLE;
static app_event_t pending_events = APP_EVENT_NONE;


static int selected_menu = 0;
static int scan_selected = 0;
scan_mode_t current_scan_mode = SCAN_MODE_MANPC;


static int last_applied_menu_focus = -1;
static int last_applied_scan_focus = -1;
static int last_applied_setting_focus = -1;
static volatile bool setting_view_refresh_pending = false;

#define BRAIN_SETTING_ITEM_COUNT   10
static int g_setting_index = 0;


static battery_level_t current_battery_level = BATTERY_LEVEL_EMPTY;


// مقداردهی اولیه پیش‌فرض مطابق با معماری پروژه
static system_settings_t g_settings = {
    .auto_cal = false,
    .auto_cal_pls = 10,
    .puls_max = 100,
    .delay_time = 500,
    .stop_trg = true,
    .beep = true,
    .bl_auto_off = false,
    .bl_auto_connect = true,
    .bl_pass = 1234,
    .bl_name = "MAGI_ESP"
};


// -------------------------
// External symbols
// -------------------------
extern volatile bool splash_done;

extern void menu_set_focused_index(int index);
extern void scan_set_focused_index(int index);

// -------------------------
// Public getters for state
// -------------------------
int brain_get_scan_selected(void)
{
    return scan_selected;
}

scan_mode_t brain_get_scan_mode(void)
{
    return current_scan_mode;
}

battery_level_t brain_get_battery_level(void)
{
    return current_battery_level;
}

uint8_t brain_get_battery_percent(void)
{
    return battery_process_get_percent();
}


scan_sub_state_t brain_get_scan_sub_state(void)
{
    return current_scan_sub_state;
}

void brain_update_battery(void)
{
    battery_level_t old_level = current_battery_level;

    battery_process_update();
    current_battery_level = battery_process_get_level();

    if (current_battery_level != old_level) {
        brain_emit_event(APP_EVENT_BATTERY_CHANGED);
    }
}


static int brain_wrap_setting_index(int index)
{
    while(index < 0) {
        index += BRAIN_SETTING_ITEM_COUNT;
    }
    while(index >= BRAIN_SETTING_ITEM_COUNT) {
        index -= BRAIN_SETTING_ITEM_COUNT;
    }
    return index;
}
int brain_get_setting_index(void)
{
    return g_setting_index;
}

void brain_set_setting_index(int index)
{
    g_setting_index = brain_wrap_setting_index(index);
}


void brain_setting_next(void)
{
    brain_set_setting_index(g_setting_index + 1);
}

void brain_setting_prev(void)
{
    brain_set_setting_index(g_setting_index - 1);
}

void brain_request_setting_view_refresh(void)
{
    setting_view_refresh_pending = true;
}

// تابع دسترسی Read-Only برای UI
const system_settings_t* brain_get_settings(void)
{
    return &g_settings;
}

//-------------------------
//event Function
//-------------------------
void brain_emit_event(app_event_t event)
{
    pending_events |= event;
}

app_event_t brain_consume_events(void)
{
    app_event_t events = pending_events;
    pending_events = APP_EVENT_NONE;
    return events;
}



// -------------------------
// Internal helpers
// -------------------------



// تغییر مقدار بولین‌ها بر اساس اندیس منو
void brain_set_bool_setting(setting_item_index_t index, bool value)
{
    bool updated = false;
    switch (index) {
        case SETTING_ITEM_AUTOCAL:
            if (g_settings.auto_cal != value) {
                g_settings.auto_cal = value;
                updated = true;
            }
            break;
        case SETTING_ITEM_STOP_TRG:
            if (g_settings.stop_trg != value) {
                g_settings.stop_trg = value;
                updated = true;
            }
            break;
        case SETTING_ITEM_BEEP:
            if (g_settings.beep != value) {
                g_settings.beep = value;
                updated = true;
            }
            break;
        case SETTING_ITEM_BL_AUTO_OFF:
            if (g_settings.bl_auto_off != value) {
                g_settings.bl_auto_off = value;
                updated = true;
            }
            break;
        case SETTING_ITEM_BL_AUTO_CONNECT:
            if (g_settings.bl_auto_connect != value) {
                g_settings.bl_auto_connect = value;
                updated = true;
            }
            break;
        default:
            ESP_LOGW(TAG, "Attempted to set non-boolean or invalid index %d as bool", index);
            return;
    }

    if (updated) {
        ESP_LOGI(TAG, "Setting index %d updated to: %s", index, value ? "ON" : "OFF");
        // در صورت نیاز به ذخیره‌سازی فوری در فلش/NVS:
        // settings_save_to_nvs(&g_settings);
    }
}

// تغییر وضعیت Toggle برای بولین‌ها
void brain_toggle_bool_setting(setting_item_index_t index)
{
    const system_settings_t *s = brain_get_settings();
    switch (index) {
        case SETTING_ITEM_AUTOCAL:
            brain_set_bool_setting(index, !s->auto_cal);
            break;
        case SETTING_ITEM_STOP_TRG:
            brain_set_bool_setting(index, !s->stop_trg);
            break;
        case SETTING_ITEM_BEEP:
            brain_set_bool_setting(index, !s->beep);
            break;
        case SETTING_ITEM_BL_AUTO_OFF:
            brain_set_bool_setting(index, !s->bl_auto_off);
            break;
        case SETTING_ITEM_BL_AUTO_CONNECT:
            brain_set_bool_setting(index, !s->bl_auto_connect);
            break;
        default:
            break;
    }
}



static lv_obj_t *brain_get_page_root(app_page_t page)
{
    switch (page)
    {
        case PAGE_SPLASH:
            return ui_Screen1;

        case PAGE_MAIN_MENU:
            return ui_MainMenu;

        case PAGE_SCAN:
            return ui_ScanMenu;

        case PAGE_SCAN_PAGE:
            return ui_ScanPage;

        case PAGE_SEND:
            return ui_SendData;

        case PAGE_MEMORY:
            return ui_Memory;

        case PAGE_SETTING:
            return ui_Setting;

        case PAGE_ABOUT:
            return ui_About;

        default:
            return NULL;
    }
}

static bool brain_is_page_ready(app_page_t page)
{
    switch (page)
    {
        case PAGE_SPLASH:
            return ui_Screen1_is_ready();

        case PAGE_MAIN_MENU:
            return ui_MainMenu_is_ready();

        case PAGE_SCAN:
            return ui_ScanMenu_is_ready();

        case PAGE_SCAN_PAGE:
            return ui_ScanPage_is_ready();

        case PAGE_SEND:
            return ui_SendData_is_ready();

        case PAGE_MEMORY:
            return ui_Memory_is_ready();

        case PAGE_SETTING:
            return ui_Setting_is_ready();

        case PAGE_ABOUT:
            return ui_About_is_ready();

        default:
            return false;
    }
}

static void brain_destroy_page(app_page_t page)
{
    switch (page)
    {
        case PAGE_SPLASH:
            if (ui_Screen1_is_ready()) {
                ui_Screen1_cleanup_and_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SPLASH");
            }
            break;

        case PAGE_MAIN_MENU:
            if (ui_MainMenu_is_ready()) {
                ui_MainMenu_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_MAIN_MENU");
            }
            break;

        case PAGE_SCAN:
            if (ui_ScanMenu_is_ready()) {
                ui_ScanMenu_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SCAN");
            }
            break;

        case PAGE_SCAN_PAGE:
            if (ui_ScanPage_is_ready()) {
                ui_ScanPage_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SCAN_PAGE");
            }
            break;

        case PAGE_SEND:
            if (ui_SendData_is_ready()) {
                ui_SendData_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SEND");
            }
            break;

        case PAGE_MEMORY:
            if (ui_Memory_is_ready()) {
                ui_Memory_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_MEMORY");
            }
            break;

        case PAGE_SETTING:
            if (ui_Setting_is_ready()) {
                ui_Setting_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_SETTING");
            }
            break;

        case PAGE_ABOUT:
            if (ui_About_is_ready()) {
                ui_About_screen_destroy();
                ESP_LOGI(TAG, "Destroyed PAGE_ABOUT");
            }
            break;

        default:
            ESP_LOGW(TAG, "Destroy not implemented for page %d", page);
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
        case PAGE_SPLASH:
            if (!ui_Screen1_is_ready()) {
                ui_Screen1_screen_init();
            }
            break;

        case PAGE_MAIN_MENU:
            if (!ui_MainMenu_is_ready()) {
                ui_MainMenu_screen_init();
            }
            break;

        case PAGE_SCAN:
            if (!ui_ScanMenu_is_ready()) {
                ui_ScanMenu_screen_init();
            }
            break;

        case PAGE_SCAN_PAGE:
            if (!ui_ScanPage_is_ready()) {
                ui_ScanPage_screen_init();
                
            }
            break;

        case PAGE_SEND:
            if (!ui_SendData_is_ready()) {
                ui_SendData_screen_init();
            }
            break;

        case PAGE_MEMORY:
            if (!ui_Memory_is_ready()) {
                ui_Memory_screen_init();
            }
            break;

        case PAGE_SETTING:
            if (!ui_Setting_is_ready()) {
                ui_Setting_screen_init();
                
            }
            break;

        case PAGE_ABOUT:
            if (!ui_About_is_ready()) {
                ui_About_screen_init();
            }
            break;

        default:
            ESP_LOGW(TAG, "Page %d is not implemented or not supported yet", page);
            return false;
    }

    if (!brain_is_page_ready(page)) {
        ESP_LOGE(TAG, "Failed to prepare page %d: page is not ready", page);
        return false;
    }

    *out_screen = brain_get_page_root(page);
    if (*out_screen == NULL) {
        ESP_LOGE(TAG, "Failed to prepare page %d: root screen is NULL", page);
        return false;
    }

    ESP_LOGI(TAG, "Prepared page %d", page);
    return true;
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

    lv_screen_load(new_screen);
    ESP_LOGI(TAG, "Loaded page %d", target_page);

    if (previous_page != target_page) {
        brain_destroy_page(previous_page);
    }

    loaded_page = target_page;

    if (target_page == PAGE_MAIN_MENU) {
        last_applied_menu_focus = -1;
    }
    else if (target_page == PAGE_SCAN) {
        last_applied_scan_focus = -1;
    }
    else if (target_page == PAGE_SETTING) {
    last_applied_setting_focus = -1;
    current_setting_state = SETTING_STATE_LIST;
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
        case PAGE_SETTING:
            {
                if (current_page == PAGE_SETTING &&
                    current_setting_state == SETTING_STATE_CLOSING) {

                    if (ui_Setting_focus_close_done()) {
                        ui_Setting_force_refresh();
                        current_setting_state = SETTING_STATE_LIST;
                        ui_Setting_update_view(g_setting_index);
                        last_applied_setting_focus = g_setting_index;
                    }
                    return;
                }

                if (current_setting_state == SETTING_STATE_LIST) {
                if (g_setting_index != last_applied_setting_focus) {
                    ui_Setting_update_view(g_setting_index);
                    last_applied_setting_focus = g_setting_index;
                    ESP_LOGD(TAG, "Applied setting focus: %d", g_setting_index);
                }
            }
            break;

            }

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
    current_scan_sub_state = SCAN_STATE_IDLE;
    pending_events = APP_EVENT_NONE;


    selected_menu = 0;
    scan_selected = 0;
    current_scan_mode = 0;

    last_applied_menu_focus = -1;
    last_applied_scan_focus = -1;
    last_applied_setting_focus = -1;
    current_setting_state = SETTING_STATE_LIST;


    g_setting_index = 0;


    battery_process_init();
    battery_process_update();
    current_battery_level = battery_process_get_level();
    esp_err_t err = app_settings_load();
    // brain_init()
    // bt_mgr_init(&(bt_mgr_config_t){
    //         .device_name = "ESP32_Scanner",
    //         .state_cb = NULL,
    //     });

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "app_settings_load failed, defaults will be used");
    }



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

        case PAGE_SCAN:         //------------ Scan Menu-------------//
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
                current_scan_mode = scan_selected;
                current_scan_sub_state = SCAN_STATE_RUNNING;
                brain_emit_event(APP_EVENT_SCAN_CHANGED);
                scan_process_init();
                current_page = PAGE_SCAN_PAGE;
                ESP_LOGI(TAG, "Scan item selected: %d, started running", current_scan_mode);
            }

            break;

           case PAGE_SCAN_PAGE:        //----------------- Scan Page ------------------//

                if (evt == KEY_BACK) {
                    if (current_scan_sub_state == SCAN_STATE_RUNNING) {
                        current_scan_sub_state = SCAN_STATE_IDLE;
                        scan_process_stop();
                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                        ESP_LOGI(TAG, "Process Stopped.");
                    }
                    else {
                        current_page = PAGE_SCAN;
                    }
                }
                else if (evt == KEY_TRIG) {
                    if (current_scan_sub_state == SCAN_STATE_RUNNING) {
                        scan_process_handle_trigger(current_scan_mode);
                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                        
                    }
                    //TO DO move to another part here jusst for test
                    brain_update_battery();
                }
                break;
        case PAGE_SETTING :     //-----------------setting page---------------//
            if (current_setting_state == SETTING_STATE_LIST) {
                if (evt == KEY_BACK) {
                    current_page = PAGE_MAIN_MENU;
                }
                else if (evt == KEY_UP) {
                    brain_setting_prev();
                }
                else if (evt == KEY_DOWN) {
                    brain_setting_next();
                }
                else if (evt == KEY_OK) {
                    ui_Setting_focus_open(g_setting_index);
                    current_setting_state = SETTING_STATE_DETAIL;
                }
            }
            else if (current_setting_state == SETTING_STATE_DETAIL) {
                if (evt == KEY_BACK) {
                    ui_Setting_focus_close(g_setting_index);
                    current_setting_state = SETTING_STATE_CLOSING;
                    ui_Setting_hide_all_details();
                    

                }
            }
            break;

        
                   

        default:
            break;
    }
}

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
    app_event_t events = brain_consume_events();

    if ((events & APP_EVENT_SCAN_CHANGED) &&
        current_page == PAGE_SCAN_PAGE &&
        ui_ScanPage_is_ready()) {
        ui_scanpage_render();
    }
    if ((events & APP_EVENT_BATTERY_CHANGED) &&
    current_page == PAGE_SCAN_PAGE &&
    ui_ScanPage_is_ready()) {
    ui_scanpage_render();
    }


}
