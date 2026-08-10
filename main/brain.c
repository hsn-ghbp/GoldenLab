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
#include "bluetooth.h"
#include <inttypes.h>
#include "esp_littlefs.h"

#define SEND_SCAN_CACHE_MAX 20U
#define SEND_MAX_SAMPLES_BUFFER 1024U 

static const char *TAG = "BRAIN";

// -------------------------
// App state
// -------------------------
static app_page_t current_page = PAGE_SPLASH;
static app_page_t loaded_page  = PAGE_SPLASH;
static scan_sub_state_t current_scan_sub_state = SCAN_STATE_IDLE;
static app_event_t pending_events = APP_EVENT_NONE;
static setting_state_t current_setting_state = SETTING_STATE_LIST;
static memory_state_t current_memory_state = MEMORY_STATE_LIST;
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
/* PAGE_SEND state: شماره نمایشی اسکن از 1 شروع می‌شود. */
static uint32_t s_send_selected_scan_number = 1U;
static uint32_t s_send_total_scan_count = 0U;
static scan_index_item_t s_send_scan_cache[SEND_SCAN_CACHE_MAX];;
static bool s_send_selection_ui_dirty = false;
// بافر استاتیک برای نگهداری داده‌ها در طول استریم بلوتوث
static int32_t s_send_stream_buffer[SEND_MAX_SAMPLES_BUFFER];
static volatile uint32_t s_pending_sent_scan_id = 0; // تعریف یک متغیر برای نگهداری درخواست علامت‌گذاری اسکن ارسال شده

// memory
static size_t g_memory_scan_count = 0;
static size_t g_memory_sent_scan_count = 0;
// volatile uint32_t g_memory_scan_count = 0;
// volatile uint32_t g_memory_sent_scan_count = 0;


/* وقتی true باشد، brain_process_ui_cmds باید label را render کند. */



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
    .bl_pass = 33138,
    .bl_name = "GOLDEN LAB"
};


// -------------------------
// External symbols
// -------------------------
extern volatile bool splash_done;

extern void menu_set_focused_index(int index);
extern void scan_set_focused_index(int index);
extern system_settings_t g_settings;



//-----------------------------
// test reading
//-----------------------------


// static void brain_log_scan_index(void)
// {
//     scan_index_item_t items[16];
//     size_t count = 0;

//     esp_err_t err = storage_littlefs_load_index(items, 16, &count);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "load_index failed: %s", esp_err_to_name(err));
//         return;
//     }

//     ESP_LOGI(TAG, "Scan index count = %u", (unsigned)count);

//     for (size_t i = 0; i < count; i++) {
//         ESP_LOGI(TAG,
//                  "[%u] id=%lu mode=%lu points=%lu ts=%lu",
//                  (unsigned)i,
//                  (unsigned long)items[i].id,
//                  (unsigned long)items[i].mode,
//                  (unsigned long)items[i].point_count,
//                  (unsigned long)items[i].timestamp_sec);
//     }
// }

static void brain_log_scan_by_id(uint32_t scan_id)
{
    scan_record_header_t header;
    uint16_t samples[MAX_SCAN_POINTS];
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
        ESP_LOGI(TAG, "sample[%u] = %u", (unsigned)i, (unsigned)samples[i]);
    }
}


// static void brain_log_last_scan(void)
// {
//     scan_index_item_t items[16];
//     size_t count = 0;

//     esp_err_t err = storage_littlefs_load_index(items, 16, &count);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "load_index failed: %s", esp_err_to_name(err));
//         return;
//     }

//     if (count == 0) {
//         ESP_LOGW(TAG, "No saved scans found");
//         return;
//     }

//     scan_index_item_t *last = &items[count - 1];

//     ESP_LOGI(TAG,
//              "Last scan => id=%lu mode=%lu points=%lu ts=%lu",
//              (unsigned long)last->id,
//              (unsigned long)last->mode,
//              (unsigned long)last->point_count,
//              (unsigned long)last->timestamp_sec);

//     brain_log_scan_by_id(last->id);
// }



// -------------------------
// Public getters for state
// -------------------------
// ۱. تابع گتر تعداد کل اسکن‌های معتبر
size_t brain_memory_get_scan_count(void)
{
    return g_memory_scan_count;
}

// ۲. تابع گتر تعداد اسکن‌های ارسال شده
size_t brain_memory_get_sent_count(void)
{
    return g_memory_sent_scan_count;
}

// ۳. تابع گتر درصد حافظه باقی‌مانده واقعی LittleFS
// uint8_t brain_memory_get_free_percent(void)
// {
//     size_t total = 0, used = 0;
//     // گرفتن اطلاعات پارتیشن LittleFS با لیبل مورد استفاده در پروژه (مثلاً "storage")
//     esp_err_t err = esp_littlefs_info("storage", &total, &used);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to get LittleFS info: %s", esp_err_to_name(err));
//         return 100; // مقدار پیش‌فرض در صورت بروز خطا
//     }

//     if (total == 0) return 0;

//     size_t free_space = total - used;
//     uint8_t free_percent = (uint8_t)((free_space * 100) / total);
//     return free_percent;
// }

battery_level_t brain_get_battery_level(void)
{
    return battery_process_get_level();
}

int brain_get_scan_selected(void)
{
    return scan_selected;
}

scan_mode_t brain_get_scan_mode(void)
{
    return current_scan_mode;
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
    g_settings.bl_pass         = 3313;

    strncpy(g_settings.bl_name, "GOLDEN LAB BRAIN", sizeof(g_settings.bl_name) - 1);
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
uint8_t brain_memory_get_free_percent(void)
{
    size_t total = 0, used = 0;
    esp_err_t err = esp_littlefs_info(STORAGE_LFS_PARTITION_LABEL, &total, &used);
    
    if (err != ESP_OK || total == 0) {
        ESP_LOGW(TAG, "Failed to get LittleFS info: %s", esp_err_to_name(err));
        return 100; // مقدار پیش‌فرض
    }

    size_t free_bytes = total - used;
    uint8_t free_percent = (uint8_t)((free_bytes * 100) / total);
    return free_percent;
}

void brain_on_scan_sent(uint32_t scan_id)
{
    if (scan_id != 0) {
        s_pending_sent_scan_id = scan_id;
        // صادر کردن یک رویداد به سیستم برای پردازش ایمن خارج از Critical Section بلوتوث
        brain_emit_event(APP_EVENT_SCAN_CHANGED); // یا هر رویدادی که باعث بررسی وضعیت در تسک اصلی می‌شود
    }
}

static void brain_memory_read_info(void)
{
    size_t count = 0;
    
    // ۱. ابتدا تعداد کل آیتم‌های موجود در ایندکس را دریافت می‌کنیم
    esp_err_t err = storage_littlefs_load_index(NULL, 0, &count);
    if (err != ESP_OK || count == 0) {
        g_memory_scan_count = 0;
        g_memory_sent_scan_count = 0;
        return;
    }

    // ۲. تخصیص حافظه برای خواندن آیتم‌ها
    scan_index_item_t *items = malloc(count * sizeof(scan_index_item_t));
    if (items == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for scan index items");
        return;
    }

    // ۳. پر کردن آرایه با اطلاعات اصلی از هدر عمومی
    size_t loaded_count = 0;
    err = storage_littlefs_load_index(items, count, &loaded_count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load scan index: %s", esp_err_to_name(err));
        free(items);
        return;
    }

    size_t active_count = 0;
    size_t sent_count = 0;

    // ۴. اعمال منطق فیلتر فلش‌ها بر روی ساختار
    for (size_t i = 0; i < loaded_count; i++) {
        if (!(items[i].flags & SCAN_FLAG_DELETED)) {
            active_count++;
            if (items[i].flags & SCAN_FLAG_SENT) {
                sent_count++;
            }
        }
    }

    g_memory_scan_count = active_count;
    g_memory_sent_scan_count = sent_count;

    free(items);
    ESP_LOGI(TAG, "Memory loaded successfully. Active: %d, Sent: %d", active_count, sent_count);
}

/*
 * اجرای واقعی حذف پس از تایید کاربر (دومین OK).
 * action: 0=حذف همه اسکن‌ها، 1=حذف آخرین اسکن، 2=حذف اسکن‌های ارسال‌شده
 * حذف منطقی است: فقط SCAN_FLAG_DELETED در ایندکس ست می‌شود.
 */
static void brain_memory_delete_confirmed(int action)
{
    if (!storage_littlefs_is_ready()) {
        ESP_LOGW(TAG, "LittleFS not ready; delete skipped");
        return;
    }

    size_t count = 0;
    esp_err_t err = storage_littlefs_load_index(NULL, 0, &count);
    if (err != ESP_OK || count == 0) {
        ESP_LOGW(TAG, "Delete skipped: index empty or unreadable");
        return;
    }

    scan_index_item_t *items = malloc(count * sizeof(scan_index_item_t));
    if (items == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for delete");
        return;
    }

    size_t loaded_count = 0;
    err = storage_littlefs_load_index(items, count, &loaded_count);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load scan index: %s", esp_err_to_name(err));
        free(items);
        return;
    }

    size_t deleted = 0;

    if (action == 1) {
        /* حذف آخرین اسکن: بیشترین id در میان اسکن‌های فعال */
        uint32_t last_id = 0;
        bool found = false;
        for (size_t i = 0; i < loaded_count; i++) {
            if (!(items[i].flags & SCAN_FLAG_DELETED) &&
                (!found || items[i].id > last_id)) {
                last_id = items[i].id;
                found = true;
            }
        }
        if (found && storage_littlefs_delete_scan(last_id) == ESP_OK) {
            deleted = 1;
        }
    } else {
        for (size_t i = 0; i < loaded_count; i++) {
            if (items[i].flags & SCAN_FLAG_DELETED) {
                continue;
            }
            if (action == 0 ||
                (action == 2 && (items[i].flags & SCAN_FLAG_SENT))) {
                if (storage_littlefs_delete_scan(items[i].id) == ESP_OK) {
                    deleted++;
                }
            }
        }
    }

    free(items);
    ESP_LOGI(TAG, "Memory delete action=%d deleted=%zu", action, deleted);

    /* به‌روزرسانی شمارنده‌های صفحه Memory پس از حذف */
    brain_memory_read_info();
}
// static void brain_memory_read_info(void)
// {
//     size_t count = 0;

//     g_memory_scan_count = 0;
//     g_memory_sent_scan_count = 0;

//     esp_err_t err = storage_littlefs_load_index(NULL, 0, &count);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG,
//                  "Failed to get scan index count: %s",
//                  esp_err_to_name(err));
//         return;
//     }

//     if (count == 0) {
//         ESP_LOGI(TAG, "Memory index is empty");
//         return;
//     }

//     scan_index_item_t *items = calloc(count, sizeof(scan_index_item_t));
//     if (items == NULL) {
//         ESP_LOGE(TAG,
//                  "Failed to allocate %zu bytes for scan index",
//                  count * sizeof(scan_index_item_t));
//         return;
//     }

//     size_t loaded_count = 0;

//     err = storage_littlefs_load_index(items, count, &loaded_count);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG,
//                  "Failed to load scan index: %s",
//                  esp_err_to_name(err));
//         free(items);
//         return;
//     }

//     ESP_LOGI(TAG,
//              "Index requested=%zu loaded=%zu",
//              count,
//              loaded_count);

//     size_t active_count = 0;
//     size_t sent_count = 0;

//     for (size_t i = 0; i < loaded_count; ++i) {
//         const bool is_deleted =
//             (items[i].flags & SCAN_FLAG_DELETED) != 0;

//         const bool is_sent =
//             (items[i].flags & SCAN_FLAG_SENT) != 0;

//         ESP_LOGI(TAG,
//                  "INDEX[%zu]: id=%" PRIu32
//                  " flags=0x%08" PRIX32
//                  " deleted=%d sent=%d",
//                  i,
//                  items[i].id,
//                  (uint32_t)items[i].flags,
//                  is_deleted,
//                  is_sent);

//         if (is_deleted) {
//             continue;
//         }

//         active_count++;

//         if (is_sent) {
//             sent_count++;
//         }
//     }

//     g_memory_scan_count = active_count;
//     g_memory_sent_scan_count = sent_count;

//     free(items);

//     ESP_LOGI(TAG,
//              "Memory loaded successfully. Active: %zu, Sent: %zu",
//              active_count,
//              sent_count);
// }


// size_t brain_memory_get_scan_count(void)
// {
//     return g_memory_scan_count;
// }

// size_t brain_memory_get_sent_scan_count(void)
// {
//     return g_memory_sent_scan_count;
// }

static uint32_t brain_send_effective_point_count(
    uint32_t point_count,
    uint32_t auto_calibration_pulse_count)
{
    if (point_count <= auto_calibration_pulse_count) {
        return 0U;
    }

    return point_count - auto_calibration_pulse_count;
}

static void brain_send_page_enter(void)
{
    s_send_total_scan_count = 0U;

    if (!storage_littlefs_is_ready()) {
        ESP_LOGW(TAG, "LittleFS is not ready; send scan count is 0");
    } else {
        size_t raw_count = 0;
        // ۱. ابتدا تعداد کل رکوردهای ثبت شده روی دیسک را دریافت می‌کنیم
        esp_err_t err = storage_littlefs_load_index(NULL, 0, &raw_count);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to load scan count: %s", esp_err_to_name(err));
        } else if (raw_count > 0) {
            // ۲. تخصیص موقت برای خواندن کل ایندکس‌ها از دیسک
            scan_index_item_t *temp_items = malloc(raw_count * sizeof(scan_index_item_t));
            if (temp_items != NULL) {
                size_t loaded_count = 0;
                err = storage_littlefs_load_index(temp_items, raw_count, &loaded_count);
                
                if (err == ESP_OK) {
                    size_t active_count = 0;
                    
                    // ۳. فیلتر کردن موارد حذف‌نشده و کپی کردن آنها در کش (تا سقف ظرفیت کش)
                    for (size_t i = 0; i < loaded_count; i++) {
                        if (!(temp_items[i].flags & SCAN_FLAG_DELETED)) {
                            if (active_count < SEND_SCAN_CACHE_MAX) {
                                s_send_scan_cache[active_count] = temp_items[i];
                            }
                            active_count++;
                        }
                    }
                    s_send_total_scan_count = (uint32_t)active_count;
                } else {
                    ESP_LOGE(TAG, "Failed to read index payload: %s", esp_err_to_name(err));
                }
                free(temp_items);
            } else {
                ESP_LOGE(TAG, "Failed to allocate temp memory for filtering scans");
            }
        }
    }

    // انتخاب پیش‌فرض از اولین آیتم شروع می‌شود
    s_send_selected_scan_number = 1U;
    s_send_selection_ui_dirty = true;

    ESP_LOGI(TAG, "PAGE_SEND entered: total_active=%lu, selected=%lu",
             (unsigned long)s_send_total_scan_count,
             (unsigned long)s_send_selected_scan_number);
}



static bool brain_scan_mode_should_save(scan_mode_t mode)
{
    return (mode == SCAN_MODE_AUTOMEM || mode == SCAN_MODE_MANMEM);
}

  static bool brain_scan_mode_uses_memory(int mode)
  {
      return mode == SCAN_MODE_AUTOMEM || mode == SCAN_MODE_MANMEM;
  }

bool brain_should_bluetooth_be_enabled(void)
{
    if (current_page != PAGE_SCAN_PAGE && current_page != PAGE_SEND) {
        return false;
    }
    /*
     * bl_auto_off == true:
     * Bluetooth فقط در صفحه اسکن و در مودهای ذخیره‌سازی خاموش می‌شود.
     * در صفحه ارسال داده (PAGE_SEND) همیشه روشن می‌ماند چون ارسال به PC به آن نیاز دارد.
     *
     * bl_auto_off == false:
     * Bluetooth در تمام مودها روشن می‌ماند.
     */
    if (current_page == PAGE_SCAN_PAGE &&
        g_settings.bl_auto_off &&
        brain_scan_mode_uses_memory((scan_mode_t)current_scan_mode)) {
        return false;
    }
    /// log
    // const bool in_scan_page = (current_page == PAGE_SCAN_PAGE);
    // const bool memory_mode = brain_scan_mode_uses_memory(current_scan_mode);
    // const bool result =
    //     in_scan_page &&
    //     !(g_settings.bl_auto_off && memory_mode);

    // ESP_LOGI(TAG,
    //          "BT policy: page=%d scan_mode=%d auto_off=%d memory_mode=%d should_enable=%d",
    //          current_page,
    //          current_scan_mode,
    //          g_settings.bl_auto_off,
    //          memory_mode,
    //          result);
////log
    return true;

}

static void brain_apply_bluetooth_policy(void)
{
    const bool should_enable = brain_should_bluetooth_be_enabled();
    const bool is_enabled = bluetooth_is_enabled();

    if (should_enable == is_enabled) {

        // ESP_LOGI(TAG,
        //  "BT apply: page=%d mode=%d auto_off=%d should=%d enabled=%d connected=%d",
        //  (int)current_page,
        //  (int)current_scan_mode,
        //  (int)g_settings.bl_auto_off,
        //  (int)should_enable,
        //  (int)is_enabled,
        //  (int)bluetooth_is_connected());
        return;
    }

    if (should_enable) {
        esp_err_t err = bluetooth_enable();

        if (err != ESP_OK) {
            ESP_LOGE(TAG,
                     "Failed to enable Bluetooth: %s",
                     esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Bluetooth enabled by scan-mode policy");
        }
    } else {
        esp_err_t err = bluetooth_disable();

        if (err != ESP_OK) {
            ESP_LOGE(TAG,
                     "Failed to disable Bluetooth: %s",
                     esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Bluetooth disabled by memory-mode policy");
        }
    }
}


static esp_err_t brain_save_current_scan(uint32_t *out_scan_id)
{
    if (!brain_scan_mode_should_save(current_scan_mode)) {
        ESP_LOGI(TAG, "Skip saving scan for mode=%d", current_scan_mode);
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint16_t point_count = 0;
    const uint16_t *samples = scan_process_get_buffer_data(&point_count);

    if (samples == NULL || point_count == 0) {
        ESP_LOGW(TAG, "No scan data available to save");
        return ESP_ERR_INVALID_STATE;
    }

    uint32_t calibration_point_count =
        scan_process_get_calibration_sample_count();

    if (calibration_point_count > point_count) {
        ESP_LOGW(TAG,
                 "Invalid calibration count: calibration=%lu, total=%u; using 0",
                 (unsigned long)calibration_point_count,
                 (unsigned)point_count);
        calibration_point_count = 0U;
    }

    storage_scan_record_t record = {
        .mode = (uint32_t)current_scan_mode,
        .timestamp_sec = 0, // فعلا RTC نداریم
        .samples = samples,
        .sample_count = point_count,
        .auto_calibration_pulse_count = calibration_point_count,
    };

    ESP_LOGI(TAG,
             "Saving scan: total=%u, calibration=%lu",
             (unsigned)record.sample_count,
             (unsigned long)record.auto_calibration_pulse_count);

    //return storage_littlefs_save_scan(&record, out_scan_id);
    esp_err_t err = storage_littlefs_save_scan(&record, out_scan_id);
    /* اگر ذخیره‌سازی ناموفق بود، پاک‌سازی فایل‌های حذف‌شده و تلاش مجدد */
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Save failed: %s, running garbage collection...", esp_err_to_name(err));

        storage_littlefs_force_cleanup_deleted();

        /* تلاش مجدد برای ذخیره‌سازی */
        err = storage_littlefs_save_scan(&record, out_scan_id);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Save failed even after GC: %s", esp_err_to_name(err));
        }
    }
    return err;
}

static void brain_stop_scan_and_save(void)
{
    uint32_t scan_id = 0;
   // esp_err_t err;

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
                        brain_memory_read_info();
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
                const bool pc_mode =
                current_scan_mode == SCAN_MODE_MANPC ||
                current_scan_mode == SCAN_MODE_AUTOPC;

                if (pc_mode && !bluetooth_is_connected()) {
                    ESP_LOGW(TAG,
                            "TRIG ignored: Bluetooth is not connected (mode=%d)",
                            current_scan_mode);
                    break;
                }
                if (current_scan_sub_state == SCAN_STATE_RUNNING) {
                    switch (current_scan_mode) {
                        case SCAN_MODE_MANPC:
                        case SCAN_MODE_MANMEM:
                        {
                            if (s_scan_trigger_phase == 0 && g_settings.auto_cal) {
                                if (scan_process_calibrate_now()) {
                                    s_scan_trigger_phase = 1;
                                    brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                    ESP_LOGI(TAG, "Manual calibration done. phase=1");
                                } else {
                                    ESP_LOGW(TAG, "Manual calibration failed");
                                }
                                break;
                            }

                            if (s_scan_trigger_phase == 0 && !g_settings.auto_cal) {
                                s_scan_trigger_phase = 1;
                            }

                            if (scan_process_capture_one_pulse()) {
                                brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                ESP_LOGI(TAG, "Manual capture done. pulse=%d",
                                        scan_process_get_pulse_count());
                            } else {
                                            ESP_LOGW(TAG, "Manual capture failed");
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

                            if (s_scan_trigger_phase == 0 && g_settings.auto_cal) {
                                if (scan_process_calibrate_now()) {
                                    s_scan_trigger_phase = 1;
                                    brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                    ESP_LOGI(TAG, "Auto calibration done. phase=1");
                                } else {
                                    ESP_LOGW(TAG, "Auto calibration failed");
                                }
                                break;
                            }

                            if (s_scan_trigger_phase == 0 && !g_settings.auto_cal) {
                                s_scan_trigger_phase = 1;
                            }

                            if (scan_process_capture_multi_pulse(g_settings.delay_time)) {
                                s_scan_trigger_phase = 2;
                                brain_emit_event(APP_EVENT_SCAN_CHANGED);
                                ESP_LOGI(TAG, "Auto scan task started. phase=2");
                            } else {
                                ESP_LOGW(TAG, "Auto scan task start failed");
                            }
                            break;
                        }

                        default:
                            ESP_LOGW(TAG, "Unknown scan mode: %d", current_scan_mode);
                            break;
                    }
                }
            }
            break;

        case PAGE_SETTING:     //-----------------setting page---------------//
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
                else if (evt == KEY_DOWN) {
                    brain_setting_detail_step(false);
                }
            }
            break;

        case PAGE_MEMORY:
            if (current_memory_state == MEMORY_STATE_LIST) {
                if (evt == KEY_UP) {
                    ui_Memory_focus_prev_btn();
                } else if (evt == KEY_DOWN) {
                    ui_Memory_focus_next_btn();
                } else if (evt == KEY_OK) {
                    /* اولین OK: فقط نمایش هشدار، حذف انجام نمی‌شود */
                    int focused = ui_Memory_get_focused_btn();
                    ui_memory_show_warning(focused);
                    current_memory_state = MEMORY_STATE_CONFIRM;
                    ESP_LOGI(TAG, "Memory: waiting confirm for action %d", focused);
                } else if (evt == KEY_BACK) {
                    current_page = PAGE_MAIN_MENU;
                }
            }
            else if (current_memory_state == MEMORY_STATE_CONFIRM) {
                if (evt == KEY_OK) {
                    /* دومین OK: تایید و اجرای حذف */
                    int focused = ui_Memory_get_focused_btn();
                    brain_memory_delete_confirmed(focused);
                    current_memory_state = MEMORY_STATE_LIST;
                    ui_Memory_render(); /* به‌روزرسانی شمارنده‌ها + مخفی کردن هشدار */
                } else if (evt == KEY_BACK) {
                    /* لغو حذف و بستن هشدار */
                    ui_memory_set_warning_visible(false);
                    current_memory_state = MEMORY_STATE_LIST;
                    ESP_LOGI(TAG, "Memory: delete canceled");
                }
                /* در حالت تایید، UP/DOWN نادیده گرفته می‌شود */
            }
            break;
        case PAGE_SEND:              //----------------Send Data Page -------------------//
            if (evt == KEY_BACK) {
                current_page = PAGE_MAIN_MENU;
            }
            else if (evt == KEY_UP) {
                if (s_send_total_scan_count > 0U) {

                    /*
                    * حرکت حلقوی:
                    * 1 -> total
                    * 2 -> 1
                    * ...
                    */
                    if (s_send_selected_scan_number <= 1U) {
                        s_send_selected_scan_number = s_send_total_scan_count;
                    } else {
                        s_send_selected_scan_number--;
                    }

                    s_send_selection_ui_dirty = true;

                    ESP_LOGI(TAG, "Send selection: %lu/%lu",
                            (unsigned long)s_send_selected_scan_number,
                            (unsigned long)s_send_total_scan_count);
                }
            }
            else if (evt == KEY_DOWN) {
                if (s_send_total_scan_count > 0U) {

                    /*
                    * حرکت حلقوی:
                    * total -> 1
                    * 1 -> 2
                    * ...
                    */
                    if (s_send_selected_scan_number >= s_send_total_scan_count) {
                        s_send_selected_scan_number = 1U;
                    } else {
                        s_send_selected_scan_number++;
                    }

                    s_send_selection_ui_dirty = true;

                    ESP_LOGI(TAG, "Send selection: %lu/%lu",
                            (unsigned long)s_send_selected_scan_number,
                            (unsigned long)s_send_total_scan_count);
                }
            }
             else if (evt == KEY_OK) {
                // بررسی اینکه آیا سیستم در حال حاضر مشغول ارسال است یا خیر
                if (bluetooth_stream_is_active()) {
                    ESP_LOGW(TAG, "Transmission in progress. Please wait...");
                    break;
                }

                ESP_LOGI(TAG, "KEY_OK pressed. Preparing to transmit scan %lu/%lu...", 
                        (unsigned long)s_send_selected_scan_number, 
                        (unsigned long)s_send_total_scan_count);

                if (!bluetooth_is_connected()) {
                    ESP_LOGW(TAG, "Transmission aborted: Bluetooth Classic is NOT connected.");
                    break;
                }

                if (s_send_total_scan_count == 0 || s_send_selected_scan_number > s_send_total_scan_count) {
                    ESP_LOGW(TAG, "No scans available or invalid selection.");
                    break;
                }

                // پیدا کردن ID واقعی اسکن انتخاب شده از کش
                uint32_t target_scan_id = s_send_scan_cache[s_send_selected_scan_number - 1].id;
                
                // خواندن هدر اسکن برای تعیین تعداد پالس‌ها
                scan_record_header_t scan_hdr = {0};
                esp_err_t err = storage_littlefs_load_scan_header(target_scan_id, &scan_hdr);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to load scan header for ID %lu: %s", (unsigned long)target_scan_id, esp_err_to_name(err));
                    break;
                }

                uint32_t point_count = scan_hdr.point_count;
                if (point_count == 0) {
                    ESP_LOGW(TAG, "Scan record has 0 points.");
                    break;
                }

                if (point_count > SEND_MAX_SAMPLES_BUFFER) {
                    ESP_LOGW(TAG, "Scan point count (%lu) exceeds buffer limit (%u). Truncating.", 
                            (unsigned long)point_count, SEND_MAX_SAMPLES_BUFFER);
                    point_count = SEND_MAX_SAMPLES_BUFFER;
                }

                // بافر موقت برای خواندن داده خام uint16_t از LittleFS
                uint16_t *raw_samples = malloc(point_count * sizeof(uint16_t));
                if (raw_samples == NULL) {
                    ESP_LOGE(TAG, "Failed to allocate temporary buffer for raw samples");
                    break;
                }

                size_t loaded_count = 0;
                err = storage_littlefs_load_scan_samples(target_scan_id, raw_samples, point_count, &loaded_count);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to load scan samples: %s", esp_err_to_name(err));
                    free(raw_samples);
                    break;
                }

                // کپی و کست به بافر استاتیک انتقال
                for (size_t i = 0; i < loaded_count; i++) {
                    s_send_stream_buffer[i] = (int32_t)raw_samples[i];
                }

                // آزادسازی بافر موقت بلافاصله پس از کپی
                free(raw_samples);

                ESP_LOGI(TAG, "Starting Bluetooth stream for %d points...", (int)loaded_count);

                // آغاز جریان ارسال استریم
                err = bluetooth_send_int32_stream_begin(target_scan_id, s_send_stream_buffer, loaded_count);
                if (err == ESP_OK) {
                    //s_send_is_streaming = true;
                    ESP_LOGI(TAG, "Bluetooth stream started successfully.");
                } else {
                    ESP_LOGE(TAG, "Failed to start Bluetooth stream: %s", esp_err_to_name(err));
                }
            }
            break;


        default:
            break;
    }

    //brain_update_battery();
}


// void brain_process_ui_cmds(void)
// {
//     static app_page_t last_logged_current = -1;
//     static app_page_t last_logged_loaded = -1;
//     brain_apply_bluetooth_policy();
//     if (current_page == PAGE_SCAN_PAGE &&
//     ui_ScanPage_is_ready()) {
//     ui_ScanPage_update_bluetooth_icon();
// }

//     if (current_page != last_logged_current || loaded_page != last_logged_loaded) {
//         ESP_LOGW(TAG, "STATE current=%d loaded=%d splash_done=%d",
//                  current_page, loaded_page, splash_done);
//         last_logged_current = current_page;
//         last_logged_loaded = loaded_page;
//     }

//     if (current_page != loaded_page) {
//         ESP_LOGW(TAG, "TRANSITION requested: %d -> %d", loaded_page, current_page);

//         if (!brain_transition_to_page(current_page)) {
//             ESP_LOGW(TAG, "Failed to transition to page %d, reverting to %d",
//                      current_page, loaded_page);
//             current_page = loaded_page;
//         }
//     }
//     brain_apply_bluetooth_policy();
//     brain_apply_focus_if_needed();

//     // ----------------------------------------------------
//     // کد جدید برای بروزرسانی صفحه PAGE_SEND
//     // ----------------------------------------------------
//     if (current_page == PAGE_SEND && ui_SendData_is_ready()) {
//         size_t total_scans = 0;
        
//         // اگر حافظه LittleFS مونت و آماده است، تعداد کل را بدون مصرف رم اضافه می‌خوانیم
//         if (storage_littlefs_is_ready()) {
//             esp_err_t err = storage_littlefs_load_index(NULL, 0, &total_scans);
//             if (err != ESP_OK) {
//                 ESP_LOGE(TAG, "Failed to load index from LittleFS: %d", err);
//                 total_scans = 0;
//             }
//         } else {
//             ESP_LOGW(TAG, "LittleFS is not ready when entering PAGE_SEND");
//         }
        
//         // ارسال مقدار به لایه نمایش (UI)
//         ui_SendData_update_scan_count((uint32_t)total_scans);
//         ui_SendData_update_scan_number(s_send_selected_scan_number);
     
//     }
//     // ----------------------------------------------------

//     app_event_t events = brain_consume_events();

//     if ((events & APP_EVENT_SCAN_CHANGED) &&
//         current_page == PAGE_SCAN_PAGE &&
//         current_scan_sub_state == SCAN_STATE_RUNNING &&
//         s_scan_trigger_phase == 2 &&
//         (current_scan_mode == SCAN_MODE_AUTOPC || current_scan_mode == SCAN_MODE_AUTOMEM)) {

//         if (g_settings.puls_max > 0 &&
//             scan_process_get_pulse_count() >= g_settings.puls_max) {
//             ESP_LOGI(TAG, "Auto scan reached puls_max=%d", g_settings.puls_max);
//             brain_stop_scan_and_save();

//             // چون brain_stop_scan_and_save خودش event تولید می‌کند،
//             // فعلاً از render همین سیکل خارج می‌شویم
//             return;
//         }
//     }

//     if ((events & APP_EVENT_SCAN_CHANGED) &&
//         current_page == PAGE_SCAN_PAGE &&
//         ui_ScanPage_is_ready()) {
//         ui_scanpage_render();
//     }

//     if ((events & APP_EVENT_BATTERY_CHANGED) &&
//         current_page == PAGE_SCAN_PAGE &&
//         ui_ScanPage_is_ready()) {
//         ui_scanpage_render();
//     }
//       if (ui_ScanPage_is_ready() && ui_Blutooth) {
//       if (bluetooth_is_enabled()) {
//           lv_obj_clear_flag(ui_Blutooth, LV_OBJ_FLAG_HIDDEN);
//       } else {
//           lv_obj_add_flag(ui_Blutooth, LV_OBJ_FLAG_HIDDEN);
//       }
//   }

// }
void brain_process_ui_cmds(void)
{
    // if (s_pending_sent_scan_id != 0) {
    //     uint32_t target_id = s_pending_sent_scan_id;
    //     s_pending_sent_scan_id = 0; // ریست کردن برای جلوگیری از اجرای مجدد

    //     ESP_LOGI("BRAIN", "Marking scan %lu as sent in LittleFS...", (unsigned long)target_id);
        
    //     // در گام‌های بعدی، این تابع را در storage_littlefs پیاده‌سازی می‌کنیم:
    //      storage_littlefs_mark_scan_sent(target_id, true);
    // }
    if (s_pending_sent_scan_id != 0) {
    const uint32_t target_id = s_pending_sent_scan_id;

    ESP_LOGI(TAG,
             "Marking scan %lu as sent in LittleFS...",
             (unsigned long)target_id);

    esp_err_t err =
        storage_littlefs_mark_scan_sent(target_id, true);

    if (err == ESP_OK) {
            /*
            * فقط پس از ذخیره موفق پاک شود.
            * در صورت خطا امکان تلاش مجدد باقی می‌ماند.
            */
            s_pending_sent_scan_id = 0;

            ESP_LOGI(TAG,
                    "Scan %lu marked as sent successfully",
                    (unsigned long)target_id);

            /*
            * به‌روزرسانی cache آماری صفحه Memory.
            */
            brain_memory_read_info();

            /*
            * به‌روزرسانی flag در cache صفحه Send تا چک‌باکس «ارسال شده»
            * بلافاصله و بدون خروج از صفحه نمایش داده شود.
            */
            for (uint32_t i = 0U;
                 i < s_send_total_scan_count && i < SEND_SCAN_CACHE_MAX;
                 i++) {
                if (s_send_scan_cache[i].id == target_id) {
                    s_send_scan_cache[i].flags |= SCAN_FLAG_SENT;
                    if (current_page == PAGE_SEND) {
                        s_send_selection_ui_dirty = true;
                    }
                    break;
                }
            }
        } else {
            ESP_LOGE(TAG,
                    "Failed to mark scan %lu as sent: %s",
                    (unsigned long)target_id,
                    esp_err_to_name(err));
        }
    }
    static app_page_t last_logged_current = -1;
    static app_page_t last_logged_loaded = -1;
    
    brain_apply_bluetooth_policy();
    
    if (current_page == PAGE_SCAN_PAGE && ui_ScanPage_is_ready()) {
        ui_ScanPage_update_bluetooth_icon();
    }

    if (current_page != last_logged_current || loaded_page != last_logged_loaded) {
        ESP_LOGW(TAG, "STATE current=%d loaded=%d splash_done=%d",
                 current_page, loaded_page, splash_done);
        last_logged_current = current_page;
        last_logged_loaded = loaded_page;
    }

    if (current_page != loaded_page) {
        ESP_LOGW(TAG, "TRANSITION requested: %d -> %d", loaded_page, current_page);

        //app_page_t prev_page = loaded_page;
        if (!brain_transition_to_page(current_page)) {
            ESP_LOGW(TAG, "Failed to transition to page %d, reverting to %d",
                     current_page, loaded_page);
            current_page = loaded_page;
        } else {
            /* فقط در صورت تغییر صفحه موفق به PAGE_SEND، مقداردهی اولیه انجام می‌شود */
            if (current_page == PAGE_SEND) {
                brain_send_page_enter();
            }
        }
    }
    
    brain_apply_bluetooth_policy();
    brain_apply_focus_if_needed();

    // ----------------------------------------------------
    // کد بهینه‌سازی‌شده برای بروزرسانی صفحه PAGE_SEND بدون تکرار دائم دیسک
    // ----------------------------------------------------
    if (current_page == PAGE_SEND && ui_SendData_is_ready()) {
        if (s_send_selection_ui_dirty) {
            ui_SendData_update_scan_count(s_send_total_scan_count);
            ui_SendData_update_scan_number(s_send_selected_scan_number);
            if (s_send_total_scan_count > 0U &&
            s_send_selected_scan_number > 0U &&
            s_send_selected_scan_number <= s_send_total_scan_count) {

            const scan_index_item_t *current_scan =
                &s_send_scan_cache[
                    s_send_selected_scan_number - 1U];
            
             // ۲. استخراج جزئیات از کش و رندر روی پنل
            //if (s_send_total_scan_count > 0U && s_send_selected_scan_number <= s_send_total_scan_count) {
             //   scan_index_item_t *current_scan = &s_send_scan_cache[s_send_selected_scan_number - 1U];
                
                // تولید متون برای سه پارامتر
                char mode_buf[32];
                char pulse_buf[16];
                char time_buf[32];
                
                // فرمت‌دهی نوع اسکن
                if (current_scan->mode == 2) {
                    snprintf(mode_buf, sizeof(mode_buf), "اتوماتیک");
                }
                else if (current_scan->mode == 3) {
                    snprintf(mode_buf, sizeof(mode_buf), "دستی");
                }
                else {
                    snprintf(mode_buf, sizeof(mode_buf), "نامشخص");
                }

                
                // فرمت‌دهی پالس‌ها
                uint32_t displayed_point_count =
                brain_send_effective_point_count(
                    current_scan->point_count,
                    current_scan->auto_calibration_pulse_count);
                    ESP_LOGI(
                        TAG,
                        "Send UI: total=%lu, calibration=%lu, displayed=%lu",
                        (unsigned long)current_scan->point_count,
                        (unsigned long)current_scan->auto_calibration_pulse_count,
                        (unsigned long)displayed_point_count);
                snprintf(pulse_buf, sizeof(pulse_buf), "%lu", (unsigned long)displayed_point_count);
                
                // فرمت‌دهی زمان ثبت اسکن (نمایش تاریخ یا ثانیه‌های خام بر اساس نیاز شما)
                // در اینجا برای نمونه ثانیه را به دقیقه تبدیل می‌کنیم
                uint32_t minutes = current_scan->timestamp_sec / 60;
                uint32_t seconds = current_scan->timestamp_sec % 60;
                snprintf(time_buf, sizeof(time_buf), "%02lu:%02lu", (unsigned long)minutes, (unsigned long)seconds);
                
                ui_SendData_update_scan_details(mode_buf, time_buf, pulse_buf);
                ui_SendData_update_sent_status(
                    (current_scan->flags & SCAN_FLAG_SENT) != 0U);
            } else {
                // اگر اسکنی موجود نبود نمایش مقادیر پیش‌فرض یا خط تیره
                ui_SendData_update_scan_details("---", "--/--", "0");
                ui_SendData_update_sent_status(false);
            }

            s_send_selection_ui_dirty = false;

        }
        
    }
    // ----------------------------------------------------

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

        //ui_scanpage_render();
    
    
    if (ui_ScanPage_is_ready() && ui_Blutooth) {
        if (bluetooth_is_enabled()) {
            lv_obj_clear_flag(ui_Blutooth, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(ui_Blutooth, LV_OBJ_FLAG_HIDDEN);
        }
    }
}




