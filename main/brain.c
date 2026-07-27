//brain.c
#include "brain.h"
#include <stdbool.h>
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"
#include <string.h>
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
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "storage_littlefs.h"



static const char *TAG = "BRAIN";

// -------------------------
// App state
// -------------------------
static app_page_t current_page = PAGE_SPLASH;
static app_page_t loaded_page  = PAGE_SPLASH;
static scan_sub_state_t current_scan_sub_state = SCAN_STATE_IDLE;
static app_event_t pending_events = APP_EVENT_NONE;
static setting_state_t current_setting_state = SETTING_STATE_LIST;
static int selected_menu = 0;
static int scan_selected = 0;
scan_mode_t current_scan_mode = SCAN_MODE_MANPC;
static int last_applied_menu_focus = -1;
static int last_applied_scan_focus = -1;
static int last_applied_setting_focus = -1;
static const char *NVS_NS = "brain_cfg";
static const char *NVS_KEY_SETTINGS = "settings";
static int g_setting_index = 0;
static battery_level_t current_battery_level = BATTERY_LEVEL_EMPTY;
static int s_scan_trigger_phase = 0;
/*
phase:
0 = هنوز شروع فاز اسکن نشده
1 = اگر auto_cal فعال بوده، کالیبراسیون انجام شده
2 = task اتوماتیک شروع شده
*/

static portMUX_TYPE s_event_lock = portMUX_INITIALIZER_UNLOCKED;

// مقداردهی اولیه پیش‌فرض مطابق با معماری پروژه
system_settings_t g_settings = {
    .auto_cal = false,
    .auto_cal_pls = 16,
    .puls_max = 300,
    .delay_time = 500,
    .stop_trg = true,
    .beep = true,
    .bl_auto_off = false,
    .bl_auto_connect = true,
    .bl_pass = 1234,
    .bl_name = "GOLDEN LAB"
};


// -------------------------
// External symbols
// -------------------------
extern volatile bool splash_done;

extern void menu_set_focused_index(int index);
extern void scan_set_focused_index(int index);



//-----------------------------
// test reading
//-----------------------------


static void brain_log_scan_index(void)
{
    scan_index_item_t items[16];
    size_t count = 0;

    esp_err_t err = storage_littlefs_load_index(items, 16, &count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "load_index failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Scan index count = %u", (unsigned)count);

    for (size_t i = 0; i < count; i++) {
        ESP_LOGI(TAG,
                 "[%u] id=%lu mode=%lu points=%lu ts=%lu",
                 (unsigned)i,
                 (unsigned long)items[i].id,
                 (unsigned long)items[i].mode,
                 (unsigned long)items[i].point_count,
                 (unsigned long)items[i].timestamp_sec);
    }
}

static void brain_log_scan_by_id(uint32_t scan_id)
{
    scan_record_header_t header;
    int16_t samples[MAX_SCAN_POINTS];
    size_t count = 0;

    esp_err_t err = storage_littlefs_load_scan_header(scan_id, &header);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "load_scan_header(%lu) failed: %s",
                 (unsigned long)scan_id, esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG,
             "SCAN HEADER: id=%lu mode=%lu points=%lu ts=%lu",
             (unsigned long)header.id,
             (unsigned long)header.mode,
             (unsigned long)header.point_count,
             (unsigned long)header.timestamp_sec);

    err = storage_littlefs_load_scan_samples(scan_id, samples, MAX_SCAN_POINTS, &count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "load_scan_samples(%lu) failed: %s",
                 (unsigned long)scan_id, esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Loaded sample count = %u", (unsigned)count);

    for (size_t i = 0; i < count; i++) {
        ESP_LOGI(TAG, "sample[%u] = %d", (unsigned)i, samples[i]);
    }
}


static void brain_log_last_scan(void)
{
    scan_index_item_t items[16];
    size_t count = 0;

    esp_err_t err = storage_littlefs_load_index(items, 16, &count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "load_index failed: %s", esp_err_to_name(err));
        return;
    }

    if (count == 0) {
        ESP_LOGW(TAG, "No saved scans found");
        return;
    }

    scan_index_item_t *last = &items[count - 1];

    ESP_LOGI(TAG,
             "Last scan => id=%lu mode=%lu points=%lu ts=%lu",
             (unsigned long)last->id,
             (unsigned long)last->mode,
             (unsigned long)last->point_count,
             (unsigned long)last->timestamp_sec);

    brain_log_scan_by_id(last->id);
}



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
        index += SETTING_ITEM_COUNT;
    }
    while(index >= SETTING_ITEM_COUNT) {
        index -= SETTING_ITEM_COUNT;
    }
    return index;
}

static esp_err_t brain_nvs_init_once(void)
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated or version changed, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(err));
    }

    return err;
}

void brain_settings_set_defaults(void)
{
    memset(&g_settings, 0, sizeof(g_settings));

    g_settings.auto_cal        = false;
    g_settings.auto_cal_pls    = 16;
    g_settings.puls_max        = 300;
    g_settings.delay_time      = 500;
    g_settings.stop_trg        = true;
    g_settings.beep            = true;
    g_settings.bl_auto_off     = false;
    g_settings.bl_auto_connect = true;
    g_settings.bl_pass         = 1234;

    strncpy(g_settings.bl_name, "MAGI_ESP", sizeof(g_settings.bl_name) - 1);
    g_settings.bl_name[sizeof(g_settings.bl_name) - 1] = '\0';
}


bool brain_settings_save(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open(write) failed: %s", esp_err_to_name(err));
        return false;
    }

    err = nvs_set_blob(handle, NVS_KEY_SETTINGS, &g_settings, sizeof(g_settings));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "brain_settings_save failed: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Settings saved");
    return true;
}

bool brain_settings_load(void)
{
    brain_settings_set_defaults();

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NS, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_open(read) failed, using defaults: %s", esp_err_to_name(err));
        return false;
    }

    size_t required_size = sizeof(g_settings);
    err = nvs_get_blob(handle, NVS_KEY_SETTINGS, &g_settings, &required_size);
    nvs_close(handle);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "No saved settings found, writing defaults");
        brain_settings_set_defaults();
        brain_settings_save();
        return true;
    }

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "nvs_get_blob failed, using defaults: %s", esp_err_to_name(err));
        brain_settings_set_defaults();
        brain_settings_save();
        return false;
    }

    if (required_size != sizeof(g_settings)) {
        ESP_LOGW(TAG, "Settings size mismatch, resetting defaults");
        brain_settings_set_defaults();
        brain_settings_save();
        return false;
    }

    g_settings.bl_name[sizeof(g_settings.bl_name) - 1] = '\0';

    ESP_LOGI(TAG, "Settings loaded");
    return true;
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
    portENTER_CRITICAL(&s_event_lock);
    pending_events |= event;
    portEXIT_CRITICAL(&s_event_lock);
}

app_event_t brain_consume_events(void)
{
    portENTER_CRITICAL(&s_event_lock);
    app_event_t events = pending_events;
    pending_events = APP_EVENT_NONE;
    portEXIT_CRITICAL(&s_event_lock);
    return events;
}




// -------------------------
// Internal helpers
// -------------------------
static bool brain_scan_mode_should_save(scan_mode_t mode)
{
    return (mode == SCAN_MODE_AUTOMEM || mode == SCAN_MODE_MANMEM);
}


static esp_err_t brain_save_current_scan(uint32_t *out_scan_id)
{
    if (!brain_scan_mode_should_save(current_scan_mode)) {
        ESP_LOGI(TAG, "Skip saving scan for mode=%d", current_scan_mode);
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint16_t point_count = 0;
    const int16_t *samples = scan_process_get_buffer_data(&point_count);

    if (samples == NULL || point_count == 0) {
        ESP_LOGW(TAG, "No scan data available to save");
        return ESP_ERR_INVALID_STATE;
    }

    storage_scan_record_t record = {
        .mode = (uint32_t)current_scan_mode,
        .timestamp_sec = 0, // فعلا RTC نداریم
        .samples = samples,
        .sample_count = point_count,
    };

    return storage_littlefs_save_scan(&record, out_scan_id);
}

static void brain_stop_scan_and_save(void)
{
    uint32_t scan_id = 0;
    esp_err_t err;

    scan_process_stop();
    if (brain_scan_mode_should_save(current_scan_mode)) {
        esp_err_t err = brain_save_current_scan(&scan_id);

        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Scan saved successfully, id=%lu", (unsigned long)scan_id);
        } else {
            ESP_LOGE(TAG, "Failed to save scan: %s", esp_err_to_name(err));
        }
    } else {
        ESP_LOGI(TAG, "Scan stopped without saving, mode=%d", current_scan_mode);
    }

    current_scan_sub_state = SCAN_STATE_STOPPED_WAIT_BACK;
    s_scan_trigger_phase = 0;
    brain_emit_event(APP_EVENT_SCAN_CHANGED);
}


static int32_t clamp_i32(int32_t value, int32_t min, int32_t max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void brain_setting_detail_step(bool increase)
{
    bool changed = false;

    switch (g_setting_index) {
        case SETTING_ITEM_AUTOCAL:
            brain_toggle_bool_setting(SETTING_ITEM_AUTOCAL);
            return;

        case SETTING_ITEM_AUTOCAL_PLS:
            if (increase) {
                int32_t new_val = clamp_i32(g_settings.auto_cal_pls + 1, 0, 9999);
                if (new_val != g_settings.auto_cal_pls) {
                    g_settings.auto_cal_pls = new_val;
                    changed = true;
                }
            } else {
                int32_t new_val = clamp_i32(g_settings.auto_cal_pls - 1, 0, 9999);
                if (new_val != g_settings.auto_cal_pls) {
                    g_settings.auto_cal_pls = new_val;
                    changed = true;
                }
            }
            break;

        case SETTING_ITEM_PULS_MAX:
            if (increase) {
                int32_t new_val = clamp_i32(g_settings.puls_max + 10, 10, 9999);
                if (new_val != g_settings.puls_max) {
                    g_settings.puls_max = new_val;
                    changed = true;
                }
            } else {
                int32_t new_val = clamp_i32(g_settings.puls_max - 10, 10, 9999);
                if (new_val != g_settings.puls_max) {
                    g_settings.puls_max = new_val;
                    changed = true;
                }
            }
            break;

        case SETTING_ITEM_DELAY_TIME:
            if (increase) {
                int32_t new_val = clamp_i32(g_settings.delay_time + 100, 100, 9999);
                if (new_val != g_settings.delay_time) {
                    g_settings.delay_time = new_val;
                    changed = true;
                }
            } else {
                int32_t new_val = clamp_i32(g_settings.delay_time - 100, 100, 9999);
                if (new_val != g_settings.delay_time) {
                    g_settings.delay_time = new_val;
                    changed = true;
                }
            }
            break;

        case SETTING_ITEM_STOP_TRG:
            brain_toggle_bool_setting(SETTING_ITEM_STOP_TRG);
            return;

        case SETTING_ITEM_BEEP:
            brain_toggle_bool_setting(SETTING_ITEM_BEEP);
            return;

        case SETTING_ITEM_BL_AUTO_OFF:
            brain_toggle_bool_setting(SETTING_ITEM_BL_AUTO_OFF);
            return;

        case SETTING_ITEM_BL_AUTO_CONNECT:
            brain_toggle_bool_setting(SETTING_ITEM_BL_AUTO_CONNECT);
            return;

        case SETTING_ITEM_BL_PASS:
            if (increase) {
                int32_t new_val = clamp_i32(g_settings.bl_pass + 1, 0, 999999);
                if (new_val != g_settings.bl_pass) {
                    g_settings.bl_pass = new_val;
                    changed = true;
                }
            } else {
                int32_t new_val = clamp_i32(g_settings.bl_pass - 1, 0, 999999);
                if (new_val != g_settings.bl_pass) {
                    g_settings.bl_pass = new_val;
                    changed = true;
                }
            }
            break;

        case SETTING_ITEM_BL_NAME:
            // فعلاً بدون تغییر
            break;

        default:
            break;
    }

    if (changed) {
        brain_settings_save();
    }

    if (ui_Setting_is_ready() && loaded_page == PAGE_SETTING) {
        ui_Setting_render_detail(g_setting_index);
    }
}


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
        brain_settings_save();

        if (ui_Setting_is_ready() && loaded_page == PAGE_SETTING) {
            ui_Setting_render_detail(g_setting_index);
        }
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
                    current_setting_state == SETTING_STATE_OPENING) {

                    if (ui_Setting_focus_open_done()) {
                        current_setting_state = SETTING_STATE_DETAIL;
                    }
                    return;
                }

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
                    if (ui_Setting_update_view(g_setting_index)) {
                        last_applied_setting_focus = g_setting_index;
                        ESP_LOGD(TAG, "Applied setting focus: %d", g_setting_index);
                    }
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
    ESP_ERROR_CHECK(brain_nvs_init_once());
    brain_settings_load();
    ESP_ERROR_CHECK(storage_littlefs_init());
    current_page = PAGE_SPLASH;
    loaded_page  = PAGE_SPLASH;
    current_scan_sub_state = SCAN_STATE_IDLE;
    pending_events = APP_EVENT_NONE;
    s_scan_trigger_phase = 0;


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
    //esp_err_t err = app_settings_load();
    // brain_init()
    // bt_mgr_init(&(bt_mgr_config_t){
    //         .device_name = "ESP32_Scanner",
    //         .state_cb = NULL,
    //     });

    // if (err != ESP_OK) {
    //     ESP_LOGW(TAG, "app_settings_load failed, defaults will be used");
    // }



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
                current_scan_mode = (scan_mode_t)scan_selected;

                scan_process_init();
                scan_process_start(current_scan_mode);

                current_scan_sub_state = SCAN_STATE_RUNNING;
                s_scan_trigger_phase = 0;

                brain_emit_event(APP_EVENT_SCAN_CHANGED);
                current_page = PAGE_SCAN_PAGE;
                ESP_LOGI(TAG, "Brain: Scan mode=%d, sub_state=RUNNING, phase=0", current_scan_mode);


            }

            break;

           case PAGE_SCAN_PAGE:        //----------------- Scan Page ------------------//

                if (evt == KEY_BACK) {
                    if (current_scan_sub_state == SCAN_STATE_RUNNING) {
                        brain_stop_scan_and_save();
                        ESP_LOGI(TAG, "Process stopped and saved by BACK");
                    }
                    else if (current_scan_sub_state == SCAN_STATE_STOPPED_WAIT_BACK) {
                        current_scan_sub_state = SCAN_STATE_IDLE;
                        current_page = PAGE_SCAN;
                        ESP_LOGI(TAG, "Leaving scan page by second BACK");
                    }
                    else {
                        current_page = PAGE_SCAN;
                    }
                }
                else if (evt == KEY_TRIG) {
                    if (current_scan_sub_state == SCAN_STATE_RUNNING) {

                        switch (current_scan_mode) {

                            case SCAN_MODE_MANPC:
                            case SCAN_MODE_MANMEM:
                            {
                                if (g_settings.auto_cal && s_scan_trigger_phase == 0) {
                                    if (scan_process_calibrate_now()) {
                                        s_scan_trigger_phase = 1;
                                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                        ESP_LOGI(TAG, "Manual calibration done. phase=1");
                                    }
                                } else {
                                    if (!g_settings.auto_cal && s_scan_trigger_phase == 0) {
                                        s_scan_trigger_phase = 1;
                                    }

                                    if (scan_process_capture_one_pulse()) {
                                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                        ESP_LOGI(TAG, "Manual capture done. pulse=%d",
                                                scan_process_get_pulse_count());
                                    }
                                }
                                break;
                            }

                            case SCAN_MODE_AUTOPC:
                            case SCAN_MODE_AUTOMEM:
                            {
                                if (s_scan_trigger_phase == 2) {
                                    if (g_settings.stop_trg) {
                                        current_scan_sub_state = SCAN_STATE_IDLE;
                                        s_scan_trigger_phase = 0;
                                        brain_stop_scan_and_save();
                                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                        ESP_LOGI(TAG, "Auto scan stopped by TRIG");
                                    } else {
                                        ESP_LOGI(TAG, "TRIG ignored in auto mode because stop_trg=false");
                                    }
                                    break;
                                }

                                if (g_settings.auto_cal && s_scan_trigger_phase == 0) {
                                    if (scan_process_calibrate_now()) {
                                        s_scan_trigger_phase = 1;
                                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                        ESP_LOGI(TAG, "Auto calibration done. phase=1");
                                    }
                                } else {
                                    if (!g_settings.auto_cal && s_scan_trigger_phase == 0) {
                                        s_scan_trigger_phase = 1;
                                    }

                                    if (scan_process_capture_multi_pulse(g_settings.delay_time)) {
                                        s_scan_trigger_phase = 2;
                                        brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                        ESP_LOGI(TAG, "Auto scan task started. phase=2");
                                    }
                                }
                                break;
                }

                default:
                    ESP_LOGW(TAG, "Unknown scan mode: %d", current_scan_mode);
                    break;
            }
        }

        brain_update_battery();
    }
    break;

        case PAGE_SETTING :     //-----------------setting page---------------//
            
                if (current_setting_state == SETTING_STATE_OPENING ||
                    current_setting_state == SETTING_STATE_CLOSING) {
                    break;
                }
        
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
                    current_setting_state = SETTING_STATE_OPENING;
                }
            }
            else if (current_setting_state == SETTING_STATE_DETAIL) {
                if (evt == KEY_BACK) {
                    ui_Setting_focus_close(g_setting_index);
                    current_setting_state = SETTING_STATE_CLOSING;
                    ui_Setting_hide_all_details();
                }
                else if (evt == KEY_UP){
                    brain_setting_detail_step(true);
                }
                else if (evt== KEY_DOWN)
                {
                    brain_setting_detail_step(false);
                }
                
            }
            break;
        case PAGE_MEMORY:
            if (evt == KEY_OK) {
                brain_log_scan_index();
                brain_log_last_scan();
            } else if (evt == KEY_BACK) {
                current_page = PAGE_MAIN_MENU;
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
        current_scan_sub_state == SCAN_STATE_RUNNING &&
        s_scan_trigger_phase == 2 &&
        (current_scan_mode == SCAN_MODE_AUTOPC || current_scan_mode == SCAN_MODE_AUTOMEM)) {

        if (g_settings.puls_max > 0 &&
            scan_process_get_pulse_count() >= g_settings.puls_max) {
            ESP_LOGI(TAG, "Auto scan reached puls_max=%d", g_settings.puls_max);
            brain_stop_scan_and_save();

            // چون brain_stop_scan_and_save خودش event تولید می‌کند،
            // فعلاً از render همین سیکل خارج می‌شویم
            return;
        }
    }

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



