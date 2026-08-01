#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "esp_lcd_gc9a01.h"
#include "esp_log.h"
#include "lvgl.h"
#include "ui.h"
#include "pcf8574.h"
#include "brain.h"
#include "bluetooth.h"

#define TAG "MAIN"

// ── Display pins (fixed, do not change) ───────────────────────
#define LCD_HOST        SPI2_HOST
#define LCD_SCLK        18
#define LCD_MOSI        19
#define LCD_DC          21
#define LCD_CS          3
#define LCD_RST         -1
#define LCD_W           240
#define LCD_H           240
#define LCD_SPI_MHZ     40

#define LVGL_TICK_PERIOD_MS     1
#define LVGL_TASK_STACK_SIZE    (8 * 1024)
#define LVGL_TASK_PRIORITY      3
#define LVGL_BUF_HEIGHT         60
#define KEY_NAV_DEBOUNCE_MS     500



bool lvgl_lock(uint32_t timeout_ms);
void lvgl_unlock(void);


static void key_task(void *arg)
{
    while (1)
    {
        key_scan();
        key_evt_t evt = key_get();

        if (evt != KEY_NONE)
        {
            // ارسال رویداد کلید به مغز برنامه جهت تصمیم‌گیری و چاپ پیام دیباگ
            brain_handle_key(evt);
        }

        vTaskDelay(pdMS_TO_TICKS(KEY_SCAN_MS));
    }
}





static SemaphoreHandle_t lvgl_mux = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

// ── DMA transfer-done callback (on panel IO) ──────────────────
static bool notify_flush_ready(esp_lcd_panel_io_handle_t io,
        esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_display_flush_ready((lv_display_t *)user_ctx);
    return false;
}

// ── LVGL flush callback ───────────────────────────────────────
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    // GC9A01 over SPI usually needs RGB565 byte swap. If colors look wrong
    // (e.g. red/blue swapped) remove this line.
    uint32_t px = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    lv_draw_sw_rgb565_swap(px_map, px);

    esp_lcd_panel_draw_bitmap(panel_handle,
        area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
    // flush_ready is called from notify_flush_ready (on_color_trans_done).
}

// ── LVGL tick timer ───────────────────────────────────────────
static void lvgl_tick_cb(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

// ── LVGL task ─────────────────────────────────────────────────


static void lvgl_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(5));
    uint32_t delay = 5;

    while (1)
    {
        if (lvgl_mux && xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(20)) == pdTRUE)
        {
            // فراخوانی تابع جدید در brain.c برای مدیریت وضعیت نمایش صفحات
            brain_process_ui_cmds();
            ui_ScanPage_update_bluetooth_icon();
            ui_ScanPage_update_calibration_icon();
            ui_SendData_update_bluetooth_status();
            delay = lv_timer_handler();
            xSemaphoreGive(lvgl_mux);
        }

        if (delay > 30) delay = 30;
        if (delay < 5)  delay = 5;

        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}


// ── lock/unlock helpers (use these around any LVGL API call) ──
bool lvgl_lock(uint32_t timeout_ms)
{
    return xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}

void lvgl_unlock(void)
{
    xSemaphoreGive(lvgl_mux);
}
static void clear_screen_black(void)
{
    uint16_t *buf = heap_caps_malloc(LCD_W * 10 * sizeof(uint16_t), MALLOC_CAP_DMA);
    if(!buf) return;
    memset(buf, 0x00, LCD_W * 10 * sizeof(uint16_t));
    for(int y = 0; y < LCD_H; y += 10) {
        esp_lcd_panel_draw_bitmap(panel_handle, 0, y, LCD_W, y + 10, buf);
    }
    free(buf);
}
void app_main(void)
{
    // ── SPI bus ───────────────────────────────────────────────
    spi_bus_config_t bus_cfg = {
        .mosi_io_num     = LCD_MOSI,
        .miso_io_num     = -1,
        .sclk_io_num     = LCD_SCLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = LCD_W * LVGL_BUF_HEIGHT * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    // ── Panel IO ──────────────────────────────────────────────
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num       = LCD_DC,
        .cs_gpio_num       = LCD_CS,
        .pclk_hz           = LCD_SPI_MHZ * 1000 * 1000,
        .lcd_cmd_bits      = 8,
        .lcd_param_bits    = 8,
        .spi_mode          = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
        (esp_lcd_spi_bus_handle_t)LCD_HOST, &io_cfg, &io_handle));

    // ── GC9A01 panel ──────────────────────────────────────────
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = LCD_RST,
        .rgb_endian     = LCD_RGB_ENDIAN_BGR,   // BGR -> fixes red<->blue swap
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(io_handle, &panel_cfg, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    clear_screen_black();
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    vTaskDelay(pdMS_TO_TICKS(200));
   
    // ── LVGL init ─────────────────────────────────────────────
    lv_init();
   pcf8574_init();
   brain_init();
    xTaskCreate(
        key_task,
        "keys",
        4096,
        NULL,
        2,
        NULL
    );
    esp_err_t bt_init_ret = bluetooth_init(NULL);

    if (bt_init_ret != ESP_OK) {
    ESP_LOGE(TAG,
             "bluetooth_init failed: %s",
             esp_err_to_name(bt_init_ret));
} else {
    ESP_LOGI(TAG, "bluetooth_init completed");
}


    // 2 bytes/pixel for RGB565 (NOT sizeof(lv_color_t), which is 3 in LVGL9)
    size_t buf_size = LCD_W * LVGL_BUF_HEIGHT * sizeof(uint16_t);
    void *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    void *buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    assert(buf1 && buf2);

    lv_display_t *disp = lv_display_create(LCD_W, LCD_H);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    // Keep LVGL rotation at 0; orientation is handled in hardware via MADCTL.
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    lv_display_set_buffers(disp, buf1, buf2, buf_size,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Register transfer-done callback (after disp exists -> passed as user_ctx)
    const esp_lcd_panel_io_callbacks_t io_cbs = {
        .on_color_trans_done = notify_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &io_cbs, disp));

    // Tick timer
    const esp_timer_create_args_t tick_args = {
        .callback = lvgl_tick_cb,
        .name = "lvgl_tick",
    };
    esp_timer_handle_t tick_timer;
    ESP_ERROR_CHECK(esp_timer_create(&tick_args, &tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    // Mutex
    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);

    // LVGL task
    xTaskCreate(lvgl_task, "lvgl", LVGL_TASK_STACK_SIZE, NULL,
                LVGL_TASK_PRIORITY, NULL);
                

    // ── UI (Step 1: just a label to prove the pipeline works) ─
    if(lvgl_lock(1000)) {
        ui_init();
        lvgl_unlock();
        ESP_LOGI(TAG, "UI init OK");
        
    };

    
    uint8_t keys;
    if (pcf8574_read(&keys) == ESP_OK)
    {
        printf("PCF = 0x%02X\n", keys);
    } 
    
};
