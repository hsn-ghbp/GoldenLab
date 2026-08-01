# PROJECT ARCHITECTURE REFERENCE

Project:
- ESP32-C3
- ESP-IDF
- LVGL 9.3
- SquareLine Studio 1.6.0
- PCF8574 برای کلیدها
- ساختار UI چندصفحه‌ای با Brain state machine
- Bluetooth Classic (SPP) برای انتقال داده
- LittleFS برای ذخیره‌سازی داده‌های اسکن

## Core Rule

در این پروژه فقط یک منبع حقیقت برای navigation وجود دارد:

`brain.c`

فایل `main.c` نباید مستقیماً تصمیم بگیرد کدام صفحه لود شود.
فایل‌های UI مثل `ui.c`, `ui_MainMenu.c`, `ui_ScanMenu.c` فقط صفحه‌ها و ویجت‌ها را می‌سازند یا ظاهر آن‌ها را آپدیت می‌کنند.
تصمیم منطقی برای تغییر صفحه باید فقط در Brain انجام شود.

---

## Complete Execution Flow From Start

### 1. Power On → `app_main()` in `main.c`

**تابع `app_main()`** (main.c:144-264)
- ورودی: هیچ (entry point برنامه)
- وظیفه: راه‌اندازی سخت‌افزار و شروع runtime

**مراحل اجرا:**

1. **راه‌اندازی SPI Bus**
   - `spi_bus_initialize()` (main.c:147-155)
   - کانفیگ SPI2_HOST با SCLK=GPIO18, MOSI=GPIO19
   - max_transfer_size = LCD_W * LVGL_BUF_HEIGHT * 2 bytes

2. **راه‌اندازی Panel IO**
   - `esp_lcd_new_panel_io_spi()` (main.c:158-169)
   - DC=GPIO21, CS=GPIO3, pclk=40MHz
   - command bits=8, param bits=8, SPI mode=0

3. **راه‌اندازی GC9A01 Panel**
   - `esp_lcd_new_panel_gc9a01()` (main.c:172-177)
   - bits_per_pixel=16 (RGB565)
   - rgb_endian=LCD_RGB_ENDIAN_BGR

4. **تنظیمات LCD**
   - `esp_lcd_panel_reset()` (main.c:178)
   - `esp_lcd_panel_init()` (main.c:179)
   - `esp_lcd_panel_invert_color()` true (main.c:180)
   - `esp_lcd_panel_swap_xy()` false (main.c:181)
   - `esp_lcd_panel_mirror()` true, false (main.c:182)
   - `clear_screen_black()` (main.c:183-143) - پر کردن صفحه با رنگ سیاه
   - `esp_lcd_panel_disp_on_off()` true (main.c:185)

5. **راه‌اندازی LVGL**
   - `lv_init()` (main.c:189) - راه‌اندازی هسته LVGL
   - `pcf8574_init()` (main.c:190) - راه‌اندازی کنترل‌کننده کلیدها

6. **راه‌اندازی Brain**
   - `brain_init()` (main.c:191) - شامل NVS, LittleFS, state machine

7. **راه‌اندازی Bluetooth**
   - `bluetooth_init()` (main.c:200-208) - راه‌اندازی اولیه SPP

8. **ساخت Task Key**
   - `xTaskCreate(key_task, "keys", 4096, ...)` (main.c:192-199)
   - priority=2

9. **ساخت buffers LVGL**
   - `heap_caps_malloc()` برای buf1 و buf2 با MALLOC_CAP_DMA (main.c:213-214)
   - buf_size = 240 * 60 * 2 = 28,800 bytes هر buffer

10. **ساخت Display**
    - `lv_display_create(LCD_W, LCD_H)` (main.c:217)
    - `lv_display_set_color_format()` LV_COLOR_FORMAT_RGB565 (main.c:218)
    - `lv_display_set_rotation()` LV_DISPLAY_ROTATION_0 (main.c:220)
    - `lv_display_set_flush_cb()` lvgl_flush_cb (main.c:221)
    - `lv_display_set_buffers()` buf1, buf2, PARTIAL render (main.c:222-223)

11. **ثبت callback flush ready**
    - `esp_lcd_panel_io_register_event_callbacks()` (main.c:226-229)
    - on_color_trans_done = notify_flush_ready

12. **ساخت tick timer**
    - `esp_timer_create()` با callback lvgl_tick_cb (main.c:232-238)
    - `esp_timer_start_periodic()` هر 1ms

13. **ساخت mutex LVGL**
    - `xSemaphoreCreateMutex()` (main.c:241-242)

14. **ساخت Task LVGL**
    - `xTaskCreate(lvgl_task, "lvgl", LVGL_TASK_STACK_SIZE, ...)` (main.c:245-246)
    - priority=3

15. **راه‌اندازی UI**
    - `lvgl_lock(1000)` (main.c:250)
    - `ui_init()` (main.c:251) - ساخت Splash screen
    - `lvgl_unlock()` (main.c:252)

16. **خواندن اولیه کلیدها**
    - `pcf8574_read()` (main.c:259-261)

---

### 2. Splash Screen Animation

**تابع `ui_init()`** (ui.c:837-847)
- ساخت theme آبی/قرمز با `lv_theme_default_init()`
- ساخت Splash screen: `ui_Screen1_screen_init()`
- load اولیه: `lv_disp_load_scr(ui_Screen1)`

**تابع `ui_Screen1_screen_init()`** (ui/screens/ui_Screen1.c)
- ساخت root object: `ui_Screen1 = lv_obj_create(NULL)`
- ساخت label GoldenLab: `ui_GoldenLabLabel = lv_label_create(ui_Screen1)`
- ساخت arc‌ها: ui_ArcRed, ui_ArcYellow, ui_ArcBlue, ui_ArcGreen
- ساخت dot‌ها: ui_RedDot, ui_GreenDot, ui_BlueDot, ui_YellowDot
- شروع انیمیشن متن: `startGoldenLabAnimation(0)`

**تابع `startGoldenLabAnimation()`** (ui.c:495-518)
- انیمیشن fade-in متن: LV_OPA_TRANSP → LV_OPA_COVER (300ms)
- انیمیشن scale متن: scale 150 → 256 با overshoot (600ms)
- completed callback: `goldenlab_done_cb()` → `splash_finished()`

**تابع `splash_finished()`** (ui.c:471-474)
- تنظیم `splash_done = true`
- فعال شدن دریافت کلیدها

**تابع `startArcAnimations()`** (ui.c:782-830)
- چهار کمان رنگی (قرمز، سبز، آبی، زرد)
- هر کمان: 0 → 100 با duration=500ms, path=ease_out
- completed callback آخری: `arcs_draw_done_cb()` → ساخت arc_timer

**تایمر `arc_spin_scale_timer_cb()`** (ui.c:645-762)
- هر 33ms اجرا می‌شود
- چرخش arc‌ها: spin_angle += 25°
- اسکیل arc‌ها: scale_factor کم می‌شود از 1.0 به 0.03
- تغییر رنگ arc‌ها: از رنگ اصلی به طلایی (gold)
- حذف dot‌ها پس پایان انیمیشن
- پس اتمام کامل: حذف arc‌ها و `startRaysPhase()`

**تابع `startRaysPhase()`** (ui.c:573-641)
- ساخت 18 ray line به صورت تصادفی از مرکز
- انیمیشن length: مرکز → بیرون با ease_out (450ms)
- انیمیشن fade-in opacity (250ms)
- انیمیشن fade-out + حذف خط (600ms بعد fade-in)

**تابع `ui_Screen1_cleanup_and_destroy()`** (ui.c:860-894)
- حذف arc_timer
- حذف تمام انیمیشن‌ها
- حذف فیزیکی ui_Screen1 از حافظه
- NULL کردن تمام pointer‌ها

---

### 3. Key Task (خواندن کلیدها)

**تابع `key_task()`** (main.c:45-60)
- حلقه بی‌پایان با priority=2
- هر `KEY_SCAN_MS` میلی‌ثانیه:
  1. `key_scan()` - اسکن وضعیت فعلی کلیدها از PCF8574
  2. `key_get()` - دریافت event کلید (press/release)
  3. `brain_handle_key(evt)` - ارسال به Brain برای تصمیم‌گیری
  4. `vTaskDelay(KEY_SCAN_MS)`

**تابع `key_scan()`** (pcf8574.c)
- خواندن port PCF8574
- debounce با بررسی تکراری
- ذخیره state در متغیرهای static

**تابع `key_get()`** (pcf8574.c)
- برگرداندن آخرین event کلید
- برگرداندن `KEY_NONE` اگر کلیدی فشرده نشده

---

### 4. LVGL Task (پردازش UI)

**تابع `lvgl_task()`** (main.c:99-121)
- حلقه بی‌پایان با priority=3
- هر iteration:
  1. گرفتن lvgl_mux با timeout=20ms
  2. `brain_process_ui_cmds()` - پردازش فرمان‌های Brain
  3. `ui_ScanPage_update_bluetooth_icon()` - آپدیت آیکون Bluetooth
  4. `ui_ScanPage_update_calibration_icon()` - آپدیت آیکون کالیبراسیون
  5. `lv_timer_handler()` - پردازش انیمیشن‌ها و رویدادهای LVGL
  6. آزاد کردن lvgl_mux
  7. delay = 5-30ms بر اساس نتیجه lv_timer_handler

**تابع `lvgl_flush_cb()`** (main.c:78-88)
- callback برای LVGL هنگام نیاز به رندر
- swap bytes RGB565: `lv_draw_sw_rgb565_swap()`
- `esp_lcd_panel_draw_bitmap()` - ارسال bitmap به LCD

**تابع `notify_flush_ready()`** (main.c:70-75)
- callback پس از اتمام انتقال SPI
- `lv_display_flush_ready()` - اطلاع‌رسانی به LVGL

**تابع `lvgl_tick_cb()`** (main.c:91-94)
- تایمر هر 1ms
- `lv_tick_inc(1)` - آپدیت tick LVGL

**تابع `lvgl_lock()`** (main.c:125-128)
- `xSemaphoreTake(lvgl_mux, timeout)`
- برگرداندن true/false

**تابع `lvgl_unlock()`** (main.c:130-133)
- `xSemaphoreGive(lvgl_mux)`

---

### 5. Brain Initialization

**تابع `brain_init()`** (brain.c:1020-1062)
- `brain_nvs_init_once()` - راه‌اندازی NVS flash
- `brain_settings_load()` - بارگذاری تنظیمات از NVS
- `storage_littlefs_init()` - راه‌اندازی LittleFS
- تنظیم state اولیه:
  - current_page = PAGE_SPLASH
  - loaded_page = PAGE_SPLASH
  - current_scan_sub_state = SCAN_STATE_IDLE
  - s_scan_trigger_phase = 0
  - selected_menu = 0, scan_selected = 0
- `battery_process_init()` - راه‌اندازی پردازش باتری
- `battery_process_update()` - خواندن اولیه باتری

**تابع `brain_settings_load()`** (brain.c:291-331)
- `brain_settings_set_defaults()` - تنظیم مقادیر پیش‌فرض
- `nvs_open()` با NVS_READONLY
- `nvs_get_blob()` برای خواندن blob settings
- اگر not found: تنظیم defaults و save
- اگر success: restore bl_name با null-termination

**تابع `brain_settings_set_defaults()`** (brain.c:247-263)
- `memset(&g_settings, 0, ...)`
- تنظیم مقادیر پیش‌فرض:
  - auto_cal=false, auto_cal_pls=16, puls_max=300
  - delay_time=500, stop_trg=true, beep=true
  - bl_auto_off=false, bl_auto_connect=true, bl_pass=3313
  - bl_name="GOLDEN LAB BRAIN"

---

### 6. Page Navigation Flow

**تابع `brain_handle_key()`** (brain.c:1079-1313)
- ورودی: `key_evt_t evt` (KEY_NONE, KEY_OK, KEY_BACK, KEY_UP, KEY_DOWN, KEY_TRIG)
- بر اساس current_page تصمیم می‌گیرد:

**وقتی PAGE_SPLASH:**
- اگر KEY_OK و splash_done == true:
  - current_page = PAGE_MAIN_MENU
  - selected_menu = 0

**وقتی PAGE_MAIN_MENU:**
- KEY_UP: selected_menu = (selected_menu - 1 + 5) % 5
- KEY_DOWN: selected_menu = (selected_menu + 1) % 5
- KEY_OK: بر اساس selected_menu:
  - 0 → PAGE_SCAN, scan_selected=0
  - 1 → PAGE_SEND
  - 2 → PAGE_MEMORY
  - 3 → PAGE_SETTING
  - 4 → PAGE_ABOUT

**وقتی PAGE_SCAN (Scan Menu):**
- KEY_BACK → current_page = PAGE_MAIN_MENU
- KEY_UP: scan_selected = (scan_selected - 1 + 4) % 4
- KEY_DOWN: scan_selected = (scan_selected + 1) % 4
- KEY_OK:
  - current_scan_mode = (scan_mode_t)scan_selected
  - `scan_process_init()`
  - `scan_process_start(current_scan_mode)`
  - current_scan_sub_state = SCAN_STATE_RUNNING
  - s_scan_trigger_phase = 0
  - `brain_emit_event(APP_EVENT_SCAN_CHANGED)`
  - current_page = PAGE_SCAN_PAGE

**وقتی PAGE_SCAN_PAGE:**
- KEY_BACK:
  - اگر SCAN_STATE_RUNNING → `brain_stop_scan_and_save()` → SCAN_STATE_STOPPED_WAIT_BACK
  - اگر SCAN_STATE_STOPPED_WAIT_BACK → current_page = PAGE_SCAN
  - دیگر → current_page = PAGE_SCAN
- KEY_TRIG: بر اساس scan_mode:
  - MANPC/MANMEM: کالیبراسیون (اگر auto_cal) + capture_one_pulse
  - AUTOPC/AUTOMEM: کالیبراسیون (اگر auto_cal) + capture_multi_pulse(delay)
  - اگر phase=2 و stop_trg=true → توقف scan

**تابع `brain_process_ui_cmds()`** (brain.c:1316-1386)
- `brain_apply_bluetooth_policy()` - روشن/خاموش کردن Bluetooth
- آپدیت آیکون Bluetooth در ScanPage
- اگر current_page != loaded_page:
  - `brain_transition_to_page(current_page)`
  - اگر fail → current_page = loaded_page
- `brain_apply_focus_if_needed()` - آپدیت focus در menu
- consume events:
  - APP_EVENT_SCAN_CHANGED → `ui_scanpage_render()`
  - APP_EVENT_BATTERY_CHANGED → `ui_scanpage_render()`

**تابع `brain_transition_to_page()`** (brain.c:916-956)
- اگر target_page == loaded_page → return true
- `brain_prepare_page(target_page, &new_screen)`
- `lv_screen_load(new_screen)`
- `brain_destroy_page(previous_page)` - حذف صفحه قبلی
- loaded_page = target_page
- ریست focus برای صفحه جدید

**تابع `brain_prepare_page()`** (brain.c:835-914)
- بر اساس page: ساخت صفحه مربوطه
  - PAGE_SPLASH: `ui_Screen1_screen_init()`
  - PAGE_MAIN_MENU: `ui_MainMenu_screen_init()`
  - PAGE_SCAN: `ui_ScanMenu_screen_init()`
  - PAGE_SCAN_PAGE: `ui_ScanPage_screen_init()`
  - PAGE_SEND: `ui_SendData_screen_init()`
  - PAGE_MEMORY: `ui_Memory_screen_init()`
  - PAGE_SETTING: `ui_Setting_screen_init()`
  - PAGE_ABOUT: `ui_About_screen_init()`
- `brain_is_page_ready(page)` - بررسی آماده بودن
- برگرداندن root screen با `brain_get_page_root(page)`

**تابع `brain_destroy_page()`** (brain.c:769-833)
- بر اساس page: اجرای destroy مربوطه
  - PAGE_SPLASH: `ui_Screen1_cleanup_and_destroy()`
  - PAGE_MAIN_MENU: `ui_MainMenu_screen_destroy()`
  - PAGE_SCAN: `ui_ScanMenu_screen_destroy()`
  - PAGE_SCAN_PAGE: `ui_ScanPage_screen_destroy()`
  - سایر صفحات: توابع destroy مربوطه

**تابع `brain_apply_focus_if_needed()`** (brain.c:958-1015)
- PAGE_MAIN_MENU: `menu_set_focused_index(selected_menu)`
- PAGE_SCAN: `scan_set_focused_index(scan_selected)`
- PAGE_SETTING: مدیریت setting state و `ui_Setting_update_view()`

---

### 7. Scan Process Flow

**تابع `scan_process_init()`** (scan_process.c)
- ریست state‌های داخلی
- ریست buffer، pulse_count
- تنظیم calibrated = false
- تنظیم sub_state = IDLE

**تابع `scan_process_start(mode)`** (scan_process.c)
- ورودی: mode (SCAN_MODE_MANPC, AUTOPC, AUTOMEM, MANMEM)
- شروع عملیات اسکن بر اساس mode

**تابع `scan_process_stop()`** (scan_process.c)
- توقف task کپچر (اگر در حال اجرا باشد)
- ریست state به IDLE

**تابع `scan_process_calibrate_now()`** (scan_process.c)
- خواندن نمونه‌های ADC بدون سیگنال
- محاسبه offset
- ذخیره offset برای پردازش بعدی
- برگرداندن true/false

**تابع `scan_process_capture_one_pulse()`** (scan_process.c)
- خواندن نمونه‌های ADC یک پالس
- اعمال offset calibration
- محاسبه signed value, positive arc, negative arc
- محاسبه needle angle
- افزایش pulse_count
- افزودن به buffer
- برگرداندن true/false

**تابع `scan_process_capture_multi_pulse(delay_ms)`** (scan_process.c)
- ورودی: delay در میلی‌ثانیه
- ساخت FreeRTOS task برای کپچر چندپالسی
- task هر delay_ms یک پالس capture می‌کند
- شرط توقف: pulse_count >= puls_max یا فراخوانی stop

**Getter‌های Scan Process:**
- `scan_process_get_current_adc_value()` - مقدار فعلی ADC
- `scan_process_get_signed_value()` - مقدار با علامت
- `scan_process_get_positive_arc_value()` - آرک مثبت
- `scan_process_get_negative_arc_value()` - آرک منفی
- `scan_process_get_needle_angle()` - زاویه عقربه (±900 = ±90 درجه)
- `scan_process_get_pulse_count()` - تعداد پالس‌های capture شده
- `scan_process_is_calibrated()` - آیا کالیبره شده؟
- `scan_process_get_buffer_data(&count)` - داده‌های buffer

**تابع `brain_stop_scan_and_save()`** (brain.c:487-508)
- `scan_process_stop()`
- اگر mode نیاز به ذخیره‌سازی دارد:
  - `brain_save_current_scan(&scan_id)` - ذخیره در LittleFS
- current_scan_sub_state = SCAN_STATE_STOPPED_WAIT_BACK
- s_scan_trigger_phase = 0
- `brain_emit_event(APP_EVENT_SCAN_CHANGED)`

**تابع `brain_save_current_scan()`** (brain.c:463-485)
- گرفتن داده از scan_process_get_buffer_data
- ساخت storage_scan_record_t
- `storage_littlefs_save_scan()` - ذخیره در فایل

---

### 8. UI Render Flow (Scan Page)

**تابع `ui_scanpage_render()`** (ui/screens/ui_ScanPage.c)
- فراخوانی getter‌ها از scan_process
- آپدیت ویجت‌های UI:
  - `lv_arc_set_value()` برای ArcP و ArcN
  - `lv_label_set_text()` برای PulseCount, CurentValue
  - `lv_obj_set_style_transform_angle()` برای needle
  - آپدیت آیکون‌های play/stop
  - آپدیت آیکون‌های mode
  - آپدیت باتری (BatL1-BatL5)

**تابع `ui_ScanPage_update_bluetooth_icon()`** (ui/screens/ui_ScanPage.c)
- بررسی `bluetooth_is_enabled()`
- نمایش/مخفی کردن ui_Blutooth

**تابع `ui_ScanPage_update_calibration_icon()`** (ui/screens/ui_ScanPage.c)
- بررسی `scan_process_is_calibrated()`
- نمایش/مخفی کردن آیکون کالیبراسیون

---

### 9. Battery Monitoring Flow

**تابع `brain_update_battery()`** (brain.c:206-216)
- `battery_process_update()` - خواندن مقدار باتری
- `current_battery_level = battery_process_get_level()`
- اگر level تغییر کرده: `brain_emit_event(APP_EVENT_BATTERY_CHANGED)`

**تابع `brain_process_ui_cmds()`** (brain.c:1370-1374)
- اگر APP_EVENT_BATTERY_CHANGED و صفحه ScanPage:
  - `ui_scanpage_render()`

---

### 10. Bluetooth Flow

**تابع `bluetooth_init()`** (bluetooth.c:421-428)
- ذخیره device name
- برگرداندن ESP_OK

**تابع `bluetooth_enable()`** (bluetooth.c:430-546)
- `nvs_flash_init()` - راه‌اندازی NVS
- `esp_bt_controller_mem_release(ESP_BT_MODE_BLE)` - آزادسازی BLE memory
- `esp_bt_controller_init()` - راه‌اندازی controller
- `esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)` - فعال‌سازی Classic BT
- `esp_bluedroid_init_with_cfg()` - راه‌اندازی Bluedroid
- `esp_bluedroid_enable()` - فعال‌سازی Bluedroid
- `esp_bt_gap_register_callback(bt_gap_cb)` - ثبت GAP callback
- `esp_spp_register_callback(bt_spp_cb)` - ثبت SPP callback
- `esp_spp_enhanced_init()` - راه‌اندازی SPP
- `esp_bt_gap_set_device_name()` - تنظیم نام دستگاه
- `esp_bt_gap_set_pin()` - تنظیم PIN
- `esp_bt_gap_set_scan_mode()` - قابل اتصال و discoverable
- ساخت retry timer برای stream
- تنظیم s_enabled = true

**تابع `bluetooth_disable()`** (bluetooth.c:548-608)
- disconnect اگر connected
- `esp_spp_deinit()`
- `esp_bluedroid_disable()`
- `esp_bluedroid_deinit()`
- `esp_bt_controller_disable()`
- `esp_bt_controller_deinit()`
- تنظیم s_enabled = false

**تابع `bt_spp_cb()`** (bluetooth.c:304-416)
- ESP_SPP_INIT_EVT: شروع سرویس SPP
- ESP_SPP_SRV_OPEN_EVT: ثبت BDA، تنظیم connected=true، clear queue
- ESP_SPP_CLOSE_EVT: تنظیم connected=false
- ESP_SPP_WRITE_EVT: پردازش نتیجه ارسال (stream یا queue)
- ESP_SPP_CONG_EVT: پردازش congestion

**تابع `bt_gap_cb()`** (bluetooth.c:98-130)
- ESP_BT_GAP_AUTH_CMPL_EVT: ثبت success/failure
- ESP_BT_GAP_PIN_REQ_EVT: پاسخ با PIN از g_settings
- ESP_BT_GAP_MODE_CHG_EVT: ثبت تغییر mode

**تابع `bluetooth_send_int32()`** (bluetooth.c:625-635)
- push value به TX queue
- `bt_send_next_queued_packet()` - ارسال اولین بسته از queue

**تابع `bluetooth_send_int32_stream_begin()`** (bluetooth.c:671-700)
- شروع ارسال آرایه به صورت stream
- هر آیتم جداگانه با فرمت "%ld\r\n"
- pacing با WRITE_EVT

**تابع `brain_should_bluetooth_be_enabled()`** (brain.c:395-428)
- اگر صفحه فعلی PAGE_SCAN_PAGE نباشد → false
- اگر bl_auto_off=true و memory mode → false
- در غیر این صورت → true

**تابع `brain_apply_bluetooth_policy()`** (brain.c:430-460)
- بررسی should_enable
- اگر متفاوت از current state → enable/disable

---

### 11. Setting Management Flow

**تابع `brain_handle_key()` در PAGE_SETTING:**
- SETTING_STATE_LIST:
  - KEY_BACK → PAGE_MAIN_MENU
  - KEY_UP: `brain_setting_prev()`
  - KEY_DOWN: `brain_setting_next()`
  - KEY_OK: `ui_Setting_focus_open()` → SETTING_STATE_OPENING
- SETTING_STATE_DETAIL:
  - KEY_BACK: `ui_Setting_focus_close()` → SETTING_STATE_CLOSING
  - KEY_UP: `brain_setting_detail_step(true)`
  - KEY_DOWN: `brain_setting_detail_step(false)`

**تابع `brain_setting_detail_step()`** (brain.c:518-622)
- بر اساس g_setting_index:
  - SETTING_ITEM_AUTOCAL: toggle
  - SETTING_ITEM_AUTOCAL_PLS: +/- 1
  - SETTING_ITEM_PULS_MAX: +/- 10
  - SETTING_ITEM_DELAY_TIME: +/- 100
  - SETTING_ITEM_STOP_TRG: toggle
  - SETTING_ITEM_BEEP: toggle
  - SETTING_ITEM_BL_AUTO_OFF: toggle
  - SETTING_ITEM_BL_AUTO_CONNECT: toggle
  - SETTING_ITEM_BL_PASS: +/- 1
- اگر تغییر کرد: `brain_settings_save()`
- `ui_Setting_render_detail()` - آپدیت UI

**تابع `brain_set_bool_setting()`** (brain.c:626-674)
- تنظیم مقدار بولین برای ایندکس مشخص
- ذخیره در NVS
- آپدیت UI

**تابع `brain_toggle_bool_setting()`** (brain.c:677-699)
- toggle مقدار بولین

---

## Pages Summary

| Page | State Variable | Files | Description |
|------|---------------|-------|-------------|
| SPLASH | PAGE_SPLASH | ui_Screen1.c/h | Splash screen with GoldenLab animation |
| MAIN MENU | PAGE_MAIN_MENU | ui_MainMenu.c/h | Main navigation menu (5 items) |
| SCAN MENU | PAGE_SCAN | ui_ScanMenu.c/h | Scan mode selection (4 modes) |
| SCAN PAGE | PAGE_SCAN_PAGE | ui_ScanPage.c/h | Real-time scan display with arcs, needle |
| SEND | PAGE_SEND | ui_SendData.c/h | Data sending interface |
| MEMORY | PAGE_MEMORY | ui_Memory.c/h | Saved scans viewer |
| SETTING | PAGE_SETTING | ui_Setting.c/h | Settings screen with 10 items |
| ABOUT | PAGE_ABOUT | ui_About.c/h | About/credits screen |

## Key Enums

### app_page_t (brain.h:9-19)
- PAGE_NONE = 0
- PAGE_SPLASH, PAGE_MAIN_MENU, PAGE_SCAN, PAGE_SCAN_PAGE
- PAGE_SEND, PAGE_MEMORY, PAGE_SETTING, PAGE_ABOUT

### scan_sub_state_t (brain.h:24-28)
- SCAN_STATE_IDLE = 0
- SCAN_STATE_RUNNING, SCAN_STATE_STOPPED_WAIT_BACK

### scan_mode_t (brain.h:36-41)
- SCAN_MODE_MANPC = 0, AUTOPC = 1, AUTOMEM = 2, MANMEM = 3

### app_event_t (brain.h:30-34)
- APP_EVENT_NONE = 0
- APP_EVENT_SCAN_CHANGED = (1<<0)
- APP_EVENT_BATTERY_CHANGED = (1<<1)

### setting_item_index_t (brain.h:52-63)
- SETTING_ITEM_AUTOCAL = 0
- SETTING_ITEM_AUTOCAL_PLS, SETTING_ITEM_PULS_MAX
- SETTING_ITEM_DELAY_TIME, SETTING_ITEM_STOP_TRG
- SETTING_ITEM_BEEP, SETTING_ITEM_BL_AUTO_OFF
- SETTING_ITEM_BL_AUTO_CONNECT, SETTING_ITEM_BL_PASS
- SETTING_ITEM_BL_NAME

## Important External Variables

- `volatile bool splash_done` - در ui.c:38، نشان‌دهنده پایان انیمیشن Splash
- `system_settings_t g_settings` - در brain.c:58-69، تنظیمات سیستم

## Golden Rule

اگر باگی در navigation، WDT، تغییر صفحه، یا کلیدها بررسی می‌شود، اول این قانون را چک کن:

**آیا فقط `brain.c` تصمیم navigation را می‌گیرد؟**

اگر نه، ابتدا باید مسیرهای navigation موازی حذف یا به Brain واگذار شوند.

---

## Key Task Flow Diagram

```
Power On
    │
    ▼
app_main() ─────────────────────────────────────┐
    │                                           │
    ├── spi_bus_initialize()                    │
    ├── esp_lcd_new_panel_io_spi()              │
    ├── esp_lcd_new_panel_gc9a01()              │
    ├── clear_screen_black()                    │
    ├── lv_init()                               │
    ├── pcf8574_init()                          │
    ├── brain_init() ──────────────┐            │
    ├── bluetooth_init()           │            │
    ├── xTaskCreate(key_task) ────┤            │
    ├── xTaskCreate(lvgl_task) ────┤            │
    ├── lvgl_lock() + ui_init() ───┤            │
    │    └──► ui_Screen1_screen_init()          │
    │    └──► startArcAnimations()              │
    │    └──► startGoldenLabAnimation()         │
    │    └──► splash_done = true (after anim)   │
    └── lvgl_unlock()                         │
                                               │
key_task ────────────────────────────┐          │
    │                                │          │
    ├── key_scan() ──► PCF8574       │          │
    ├── key_get() ──► key_evt_t      │          │
    └── brain_handle_key(evt) ───────┤          │
         │  (updates brain state)   │          │
         ▼                          │          │
lvgl_task ─────────────────────┐     │          │
    │                         │     │          │
    ├── brain_process_ui_cmds()◄─────┤          │
    │    │                     │     │          │
    │    ├── brain_transition_to_page()         │
    │    │    └── brain_prepare_page()          │
    │    │    └── lv_screen_load()              │
    │    │    └── brain_destroy_page()          │
    │    │                                    │
    │    ├── brain_apply_focus_if_needed()     │
    │    └── ui_scanpage_render() (on event)   │
    │                                         │
    ├── ui_ScanPage_update_bluetooth_icon()   │
    ├── ui_ScanPage_update_calibration_icon() │
    └── lv_timer_handler() ──► LVGL animations│
                                               │
                                               ▼
                                    Display shows updated UI