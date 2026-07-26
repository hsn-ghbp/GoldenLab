پروژه بر پایه معماری Brain-Centric طراحی شده است.
brain تصمیم‌گیر مرکزی است و مالک stateهای page، scan mode، scan sub-state و trigger phase محسوب می‌شود.
scan_process فقط مجری است و نباید منطق تصمیم‌گیری سطح بالا داشته باشد؛ وظایفش شامل init, start, stop, calibrate_now, capture_one_pulse, capture_multi_pulse و getterهای UI است.
ui فقط renderer است؛ هیچ تصمیم عملیاتی درباره start/stop/calibrate نمی‌گیرد و صرفاً داده را از طریق render/setter نشان می‌دهد.
فازهای trigger این‌گونه‌اند: 0=ready, 1=calibrated/armed, 2=auto task running.
اگر g_settings.auto_cal فعال باشد، قبل از هر capture باید scan_process_calibrate_now() اجرا شود.
در مودهای Manual (MANPC, MANMEM) هر تریگر بعد از calibration فقط یک پالس با scan_process_capture_one_pulse() می‌گیرد.
در مودهای Auto (AUTOPC, AUTOMEM) تریگر بعد از calibration، scan_process_capture_multi_pulse(delay) را شروع می‌کند و فاز را به 2 می‌برد.
در Auto mode وقتی فاز 2 است، اگر g_settings.stop_trg=true باشد تریگر باعث توقف task می‌شود؛ در غیر این صورت تریگر نادیده گرفته می‌شود.
pulse_count فقط برای پالس‌های capture شده است و calibration نباید آن را تغییر دهد.
brain_process_ui_cmds() علاوه بر page transition و focus handling، در مود Auto شرط scan_process_get_pulse_count() >= g_settings.puls_max را بررسی می‌کند و در صورت رسیدن به حد، اسکن را متوقف می‌کند.
APP_EVENT_SCAN_CHANGED و APP_EVENT_BATTERY_CHANGED برای render صفحه Scan استفاده می‌شوند و render فقط وقتی باید انجام شود که ui_ScanPage_is_ready() true باشد.
main.c نقش bootstrap/runtime coordinator را دارد: init سخت‌افزار، LVGL، taskهای پایه و راه‌اندازی brain و ui.



ماژول main.c در این پروژه نقش bootstrap و coordinator برای UI runtime را دارد.

وظایف اصلی:
- راه‌اندازی SPI LCD و پنل GC9A01
- راه‌اندازی LVGL
- ساخت display و DMA buffers
- تعریف flush callback و transfer-done callback
- ساخت esp_timer برای lv_tick_inc
- ساخت mutex برای thread-safe access به LVGL
- اجرای lvgl_task برای:
  1) brain_process_ui_cmds()
  2) lv_timer_handler()
- اجرای key_task برای:
  1) key_scan()
  2) key_get()
  3) brain_handle_key(evt)

معماری:
- Brain تصمیم می‌گیرد
- key_task فقط ورودی را جمع می‌کند
- lvgl_task تنها محل امن اجرای commandهای UI است
- UI با ui_init() ساخته می‌شود
- هر دسترسی دیگر به LVGL باید با lvgl_lock/lvgl_unlock انجام شود

توابع مهم:
- app_main()
- key_task()
- lvgl_task()
- lvgl_flush_cb()
- notify_flush_ready()
- lvgl_tick_cb()
- lvgl_lock()
- lvgl_unlock()
- clear_screen_black()



پروژه: ESP32-C3 + ESP-IDF + LVGL 9.3 + SquareLine Studio 1.6.0

معماری:
- Brain = تصمیم‌گیر
- Scan Process = مجری عملیات
- UI = فقط نمایشگر

قوانین اصلی:
- UI هیچ منطق تصمیم‌گیری ندارد و فقط getterها را می‌خواند
- Brain مسئول trigger phase، scan mode، puls_max، stop_trg و start/stop logic است
- Scan Process فقط فرمان‌ها را اجرا می‌کند: calibration، single pulse، auto capture، buffer، UI-ready values

رفتار Scan Process:
- pulse_count فقط capture واقعی را می‌شمارد، calibration را نه
- در auto mode یک Task جداگانه پالس‌ها را با delay مشخص می‌گیرد
- بعد از هر pulse در auto mode یک event برای refresh UI ارسال می‌شود
- آخرین ADC و مقادیر signed value / positive arc / negative arc / needle angle داخل Scan Process نگه داشته می‌شود

توابع مهم Scan Process:
- scan_process_init()
- scan_process_start(mode)
- scan_process_stop()
- scan_process_calibrate_now()
- scan_process_capture_one_pulse()
- scan_process_capture_multi_pulse(delay_ms)
- getterها برای adc / arcs / angle / pulse_count / buffer

هدف فعلی:
[اینجا فقط task فعلی را بنویس؛ مثلا: اصلاح brain trigger logic / ساخت scan_process.h / اتصال UI به getterها]
////////////////////////////// جمع بندی بررسی سه ماژول مین و برین و پروسس ///////////////////////////////
[نینجا]

بسیار عالی. تمام توافقات، تحلیل‌ها و جزئیات سه ماژول اصلی را در قالب یک **Context Card واحد و منسجم** تدوین کردم. این متن فاقد جملات تکراری است، لایه‌های مسئولیت را به‌وضوح تفکیک کرده و ترتیب منطقی (از کل به جزء) را رعایت کرده است.

---

### **پروژه: سیستم کنترل هوشمند ESP32-LVGL (Brain-Centric)**

**تکنولوژی‌ها:** ESP32-C3 | ESP-IDF | LVGL 9.3 | SquareLine Studio 1.6.0 | PCF8574 (Keys)

---

#### **۱. معماری کلان (Core Architecture)**
این پروژه بر پایه تفکیک کامل مسئولیت‌ها در سه لایه طراحی شده است:
*   **Brain (تصمیم‌گیر):** مالک وضعیت سیستم (State)، مدیریت صفحات، تفسیر کلیدها و صدور فرمان‌های سطح بالا.
*   **Scan Process (مجری):** موتور اجرایی عملیات سخت‌افزاری (ADC، کالیبراسیون، تسک‌های کپچر) و پردازش داده‌های خام.
*   **UI (نمایشگر):** فاقد منطق عملیاتی؛ صرفاً داده‌های آماده را از طریق Getterها خوانده و نمایش می‌دهد.

---

#### **۲. منطق و فازهای عملیاتی اسکن**
مدیریت عملیات اسکن توسط **Trigger Phases** در ماژول `brain` کنترل می‌شود:
*   **Phase 0 (آماده):** وضعیت Idle؛ سیستم منتظر تریگر اول برای شروع است.
*   **Phase 1 (کالیبره/آماده‌باش):** کالیبراسیون انجام شده (اگر `auto_cal` فعال باشد) و سیستم آماده کپچر پالس است.
*   **Phase 2 (اجرای اتوماتیک):** تسک کپچر چندپالسی در پس‌زمینه در حال اجراست.

**قوانین حاکم بر رفتار اسکن:**
1.  **کالیبراسیون:** اگر `g_settings.auto_cal` فعال باشد، اولین تریگر در فاز ۰ منجر به اجرای `calibrate_now()` می‌شود. کالیبراسیون بر `pulse_count` اثری ندارد.
2.  **مود دستی (Manual):** هر تریگر بعد از کالیبراسیون، منجر به `capture_one_pulse()` و افزایش شمارنده می‌شود.
3.  **مود خودکار (Auto):** تریگر بعد از کالیبراسیون، تسک `capture_multi_pulse` را با تاخیر مشخص شروع می‌کند.
4.  **توقف تریگر (Stop Trigger):** در مود Auto (فاز ۲)، تریگر فقط در صورتی باعث توقف تسک می‌شود که `g_settings.stop_trg` فعال باشد.
5.  **توقف خودکار (Watchdog):** در مود Auto، اگر `pulse_count` به حد `g_settings.puls_max` برسد، `brain` دستور توقف صادر می‌کند.

---

#### **۳. تفکیک وظایف ماژول‌ها**

**ماژول `main.c` (Bootstrap & Runtime):**
*   راه‌اندازی سخت‌افزار (LCD GC9A01، SPI، DMA، PCF8574).
*   مدیریت تایمرهای سیستم (lv_tick) و Mutexهای Thread-safety برای LVGL.
*   **lvgl_task:** اجرای پردازش‌های UI (`lv_timer_handler`) و منطق فرمان‌های Brain.
*   **key_task:** پایش کلیدها و ارسال رویدادها به `brain_handle_key`.

**ماژول `brain.c` (Logic Controller):**
*   مدیریت ماشین وضعیت صفحات (Transition/Destroy/Focus).
*   مدیریت رویدادهای سیستمی (`APP_EVENT_SCAN_CHANGED` و `BATTERY`).
*   هماهنگی بین `scan_process` و `ui` (تصمیم‌گیری برای شروع/توقف بر اساس ورودی کاربر).

**ماژول `scan_process.c` (Executive Motor):**
*   انجام کالیبراسیون و کپچر پالس (تکی و تسک‌محور).
*   نگهداری بافر داده‌ها و شمارنده پالس‌ها (`pulse_count`).
*   ارائه توابع Getter برای تبدیل داده‌های خام به مقادیر قابل نمایش (Signed values، Arc angles، Needle angles).

**ماژول `ui` (Visual Layer):**
*   رندرینگ صفحات بر اساس رویدادهای دریافتی از Brain.
*   فراخوانی Getterهای `scan_process` برای به‌روزرسانی عقربه‌ها و ویجت‌ها در صفحه اسکن.

---

#### **۴. هدف فعلی (Current Task)**
*   **[اتصال نهایی توابع Getter ماژول scan_process به تابع ui_scanpage_render در لایه UI]**

---
**توصیه نهایی نینجا:** این Context Card را در ابتدای هر گفتگو کپی کنید تا من دقیقاً بدانم در کدام لایه و با چه قوانینی باید کد بنویسم. 



Context Card — ui_Setting.c

Module role:
- UI layer for settings screen in Brain-Centric architecture.
- Only displays state; does not own business logic.

Main responsibilities:
- Create and destroy the settings screen.
- Render a vertical list of setting items.
- Show/hide detail controls (switches, labels, numeric values).
- Handle animation state for list movement and detail open/close.

Initialization behavior:
- Creates root screen object: ui_Setting.
- Builds all list items as buttons.
- Builds all detail widgets upfront and hides them.
- Initializes setting_items[] with all list objects.
- Resets animation flags and visible-item cache.
- Calls ui_Setting_update_view(brain_get_setting_index()) to sync with Brain.

Destruction behavior:
- Deletes transition timer and frees its user_data.
- Cancels all active animations.
- Deletes root screen object.
- Sets all UI pointers to NULL.
- Clears setting_items[].

Important state variables:
- setting_items[SETTING_ITEM_COUNT]
- prev_visible[3]
- setting_first_layout
- setting_animating
- setting_force_update
- setting_open_anim_running
- setting_close_anim_running
- setting_open_target_index
- setting_restore_focus_idx

Dependency rules:
- Current selection comes from brain_get_setting_index().
- Setting values are rendered from Brain state.
- UI must not directly own setting logic.

Notes:
- All detail widgets are created hidden, then shown per selected item.
- Some switches are display-only; actual state changes happen in Brain / physical key handling.
- Care must be taken to keep names and show/hide logic synchronized with brain.h structures.
