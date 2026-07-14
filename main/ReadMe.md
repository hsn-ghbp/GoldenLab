# PROJECT ARCHITECTURE REFERENCE

Project:
- ESP32-C3
- ESP-IDF
- LVGL 9.3
- SquareLine Studio 1.6.0
- PCF8574 برای کلیدها
- ساختار UI چندصفحه‌ای با Brain state machine

## Core Rule

در این پروژه فقط یک منبع حقیقت برای navigation وجود دارد:

`brain.c`

فایل `main.c` نباید مستقیماً تصمیم بگیرد کدام صفحه لود شود.
فایل‌های UI مثل `ui.c`, `ui_MainMenu.c`, `ui_ScanMenu.c` فقط صفحه‌ها و ویجت‌ها را می‌سازند یا ظاهر آن‌ها را آپدیت می‌کنند.
تصمیم منطقی برای تغییر صفحه باید فقط در Brain انجام شود.

---

## Main Runtime Flow

جریان کلی برنامه:

`app_main`
-> hardware init
-> LVGL init
-> `ui_init()`
-> `brain_init()`
-> start tasks
-> `key_task`
-> `lvgl_task`
-> `brain_handle_key()`
-> `brain_process_ui_cmds()`
-> `brain_transition_to_page()`

---

## main.c Responsibilities

`main.c` مسئول orchestration است، نه navigation.

وظایف مجاز `main.c`:

- راه‌اندازی LCD / SPI / panel
- راه‌اندازی LVGL
- ساخت mutex یا lock مربوط به LVGL
- راه‌اندازی tick timer برای LVGL
- راه‌اندازی PCF8574
- راه‌اندازی key system
- ساخت taskهای اصلی مثل:
  - `key_task`
  - `lvgl_task`
- صدا زدن `ui_init()`
- صدا زدن `brain_init()`
- اجرای `lv_timer_handler()` داخل task مربوط به LVGL
- صدا زدن تابعی که فرمان‌های Brain/UI را پردازش کند

وظایف غیرمجاز `main.c`:

- تصمیم مستقیم برای رفتن به صفحه بعد
- اجرای مستقیم `lv_screen_load()` برای navigation اصلی
- نگه داشتن state موازی برای صفحه فعلی
- داشتن navigation logic جدا از `brain.c`

---

## ui.c Responsibilities

`ui.c` مسئول init سطح بالای UI است.

در `ui_init()` معمولاً این اتفاق‌ها می‌افتد:

- ساخت Splash screen:
  - `ui_Screen1_screen_init()`
- ساخت Main Menu:
  - `ui_MainMenu_screen_init()`
- ساخت Scan Menu:
  - `ui_ScanMenu_screen_init()`
- load اولیه Splash:
  - `lv_screen_load(ui_Screen1)`

نکته مهم:

`ui_init()` فقط شروع برنامه را با Splash انجام می‌دهد.
بعد از شروع برنامه، navigation اصلی نباید در `ui.c` تصمیم‌گیری شود.

---

## Splash Logic

برنامه با Splash شروع می‌شود.

در ابتدا:

- `current_page = PAGE_SPLASH`
- `loaded_page = PAGE_SPLASH`
- `splash_done = false`

در پایان animationهای Splash:

- `splash_done = true`

تا زمانی که `splash_done == false` باشد:

- ورودی کلیدها نباید باعث navigation شود
- `key_task` باید input را نادیده بگیرد یا event مؤثر تولید نکند

بعد از `splash_done == true`:

- کلیدها فعال می‌شوند
- کاربر می‌تواند با OK از Splash وارد Main Menu شود

---

## key_task Responsibilities

`key_task` مسئول خواندن کلیدها از PCF8574 است.

رفتار مورد انتظار:

- اجرای دوره‌ای، مثلاً هر 20ms
- خواندن وضعیت کلیدها
- debounce
- تشخیص press/release یا event معتبر
- نادیده گرفتن کلیدها تا قبل از `splash_done`
- ارسال event کلید به Brain، نه اجرای مستقیم navigation

قانون مهم:

`key_task` نباید مستقیماً صفحه عوض کند.
فقط باید event را به مسیر Brain بدهد.

---

## lvgl_task Responsibilities

`lvgl_task` مسئول زنده نگه داشتن LVGL است.

رفتار مورد انتظار:

- گرفتن lock/mutex مربوط به LVGL
- پردازش commandهای UI/Brain
- اجرای `lv_timer_handler()`
- آزاد کردن lock
- delay مناسب

ترتیب پیشنهادی داخل loop:

1. گرفتن lock
2. `brain_process_ui_cmds()`
3. `lv_timer_handler()`
4. آزاد کردن lock
5. `vTaskDelay(...)`

نکته:

تمام کارهای LVGL باید از مسیر امن و lock شده انجام شوند.

---

## brain.c Responsibilities

`brain.c` منبع حقیقت navigation و state machine برنامه است.

Stateهای اصلی:

- `current_page`
- `loaded_page`
- `selected_menu`

معنی stateها:

- `current_page`: صفحه‌ای که برنامه می‌خواهد به آن برود
- `loaded_page`: صفحه‌ای که واقعاً الان روی نمایشگر load شده
- `selected_menu`: آیتم انتخاب‌شده در Main Menu

قانون اصلی:

`brain_handle_key()` فقط state را تغییر می‌دهد.
`brain_process_ui_cmds()` تغییر state را به UI واقعی تبدیل می‌کند.

---

## brain_init()

در شروع برنامه:

- `current_page = PAGE_SPLASH`
- `loaded_page = PAGE_SPLASH`
- `selected_menu = MENU_SCAN` یا آیتم پیش‌فرض
- sync اولیه با UI انجام می‌شود، اگر لازم باشد

---

## brain_handle_key()

این تابع ورودی کلید را می‌گیرد و state منطقی را تغییر می‌دهد.

نمونه رفتارها:

### وقتی صفحه Splash است

اگر:

- `splash_done == true`
- کلید `OK` زده شود

آنگاه:

- `current_page = PAGE_MAIN_MENU`

### وقتی صفحه Main Menu است

کلیدهای بالا/پایین:

- `selected_menu` را تغییر می‌دهند
- UI menu focus را آپدیت می‌کنند

کلید OK:

- اگر آیتم Scan انتخاب شده:
  - `current_page = PAGE_SCAN`
- اگر آیتم‌های دیگر هنوز پیاده‌سازی نشده‌اند:
  - نباید transition واقعی انجام شود
  - یا باید reject شوند

### وقتی صفحه Scan است

کلید BACK:

- `current_page = PAGE_MAIN_MENU`

---

## brain_process_ui_cmds()

این تابع bridge بین state منطقی و LVGL واقعی است.

رفتار اصلی:

اگر:

`current_page != loaded_page`

آنگاه:

`brain_transition_to_page(current_page)`

اگر transition موفق بود:

`loaded_page = current_page`

اگر transition ناموفق بود:

`current_page = loaded_page`

قانون مهم:

این تابع باید در context امن LVGL اجرا شود، معمولاً داخل `lvgl_task` و زیر lock.

---

## brain_transition_to_page()

این تابع transition واقعی صفحه را انجام می‌دهد.

مراحل منطقی:

1. بررسی اینکه صفحه مقصد پشتیبانی می‌شود یا نه
2. آماده‌سازی صفحه مقصد با `brain_prepare_page()`
3. load کردن صفحه مقصد با `lv_screen_load(...)`
4. آپدیت `loaded_page`

در نسخه پایدار فعلی:

- destroy کردن صفحه قبلی حذف یا غیرفعال شده
- دلیل: جلوگیری از WDT و crash مرتبط با LVGL animation/draw lifecycle

قانون فعلی:

از الگوی پرریسک زیر پرهیز شود:

`load new -> destroy old`

یا:

`destroy old` نزدیک به animation/draw فعال

تا زمانی که lifecycle صفحات کاملاً امن نشده، destroy صفحه قبلی نباید فعال شود.

---

## brain_prepare_page()

این تابع صفحه مقصد را آماده می‌کند.

صفحات پشتیبانی‌شده فعلی:

- `PAGE_SPLASH`
- `PAGE_MAIN_MENU`
- `PAGE_SCAN`

صفحات unsupported فعلی:

- `PAGE_SEND`
- `PAGE_MEMORY`
- `PAGE_SETTING`
- `PAGE_ABOUT`
- هر صفحه‌ای که init/load کامل و امن ندارد

رفتار برای unsupported page:

- transition باید reject شود
- `current_page` باید به `loaded_page` برگردد
- نباید `lv_screen_load(NULL)` یا screen نامعتبر اجرا شود

---

## UI Page Files

### ui_Screen1.c / Splash

مسئول:

- ساخت Splash
- اجرای animationها
- در پایان animation:
  - `splash_done = true`

نباید مسئول navigation اصلی بعد از Splash باشد.

### ui_MainMenu.c

مسئول:

- ساخت Main Menu screen
- ساخت itemهای menu
- نمایش focus/selection
- ارائه تابع‌هایی برای آپدیت selection مثل:
  - `menu_next()`
  - `menu_prev()`
  - `menu_set_focused_index(...)`

نباید خودش تصمیم بگیرد وارد کدام صفحه شود.
تصمیم ورود به Scan یا صفحات دیگر باید در `brain.c` باشد.

### ui_ScanMenu.c

مسئول:

- ساخت Scan screen
- ساخت widgetهای مربوط به Scan
- آماده‌سازی ظاهر صفحه Scan

نباید خودش navigation اصلی را کنترل کند.

---

## Supported Navigation

فعلاً navigation پایدار پروژه:

1. Boot
2. Splash
3. OK بعد از پایان Splash
4. Main Menu
5. انتخاب Scan
6. OK
7. Scan Page
8. BACK
9. Main Menu

صفحات دیگر فعلاً نباید transition واقعی داشته باشند مگر اینکه init/load آن‌ها کامل و امن شده باشد.

---

## WDT Root Cause Reference

مشکل WDT قبلی با احتمال زیاد از transition و lifecycle صفحه‌ها بود.

ریسک‌های اصلی:

- destroy کردن صفحه قبلی در زمان نامناسب
- destroy نزدیک به animation فعال
- destroy هم‌زمان با draw/timer داخلی LVGL
- transition به صفحه unsupported
- برگشت ندادن `current_page` بعد از transition ناموفق
- وجود دو مسیر navigation جداگانه در `main.c` و `brain.c`

اصلاح پایدار فعلی:

- حذف یا غیرفعال کردن destroy صفحه قبلی
- reject کردن صفحات unsupported
- revert کردن `current_page` به `loaded_page` هنگام خطا
- متمرکز کردن navigation در Brain

---

## Future Refactor Plan

مسیر پیشنهادی بعدی، ولی فعلاً ضروری نیست:

1. تغییر signature توابع init صفحه‌ها به `bool`
   - `bool ui_MainMenu_screen_init(void)`
   - `bool ui_ScanMenu_screen_init(void)`

2. بازنویسی `brain_prepare_page()` بر اساس مقدار برگشتی init

3. اضافه کردن validation برای screen pointerها

4. اضافه کردن page lifecycle مشخص:
   - create
   - load
   - unload
   - destroy safe

5. فقط بعد از پایدار شدن lifecycle، destroy کنترل‌شده اضافه شود

---

## Golden Rule

اگر باگی در navigation، WDT، تغییر صفحه، یا کلیدها بررسی می‌شود، اول این قانون را چک کن:

آیا فقط `brain.c` تصمیم navigation را می‌گیرد؟

اگر نه، ابتدا باید مسیرهای navigation موازی حذف یا به Brain واگذار شوند.

Page Lifecycle Contract
برای جلوگیری از WDT و Crash، تمام صفحات باید از الگوی زیر پیروی کنند:

screen_init():

اشاره‌گرهای اصلی صفحه (Root و ویجت‌های کلیدی) را مقداردهی می‌کند.
اگر صفحه قبلاً ساخته شده باشد، باید قبل از ساخت مجدد، تخریب شود (یا از همان موجود استفاده کند).
is_ready():

فقط بر اساس NULL نبودن اشاره‌گرهای اصلیِ صفحه تصمیم می‌گیرد.
مثال: return (ui_Screen1 != NULL);
screen_destroy():

ویجت‌ها را با lv_obj_delete() حذف می‌کند (یا مدیریت حافظه را انجام می‌دهد).
بسیار مهم: اشاره‌گرهای اصلی را بلافاصله NULL می‌کند.
مثال: ui_Screen1 = NULL;
قانون: اگر صفحه destroy شده باشد، is_ready() باید به‌صورت خودکار و بدون نیاز به فلگِ جانبی، false برگرداند.
در brain قرار شد مدل اصلی state نگه داشته شود؛ UI فقط از getterها بخواند و خودش render کند.
برای اسکن، دو state تعریف شد: SCAN_STATE_IDLE و SCAN_STATE_RUNNING.
تصمیم گرفتیم brain یک event bitmask داشته باشد و هر تغییر مرتبط با اسکن، APP_EVENT_SCAN_CHANGED تولید کند.
قرار شد بعد از پردازش هر کلید، eventها consume شوند و اگر صفحه فعلی PAGE_SCAN_PAGE بود، فقط ui_scanpage_render() صدا زده شود.
منطق ScanPage این شد که چیزی را از خودش حدس نزند؛ فقط با brain_get_scan_mode() و brain_get_scan_sub_state() نمایش را sync کند.
نگاشت modeها را تثبیت کردیم:
0 = manpc
1 = autopc
2 = automem
3 = manmem
برای نمایش mode iconها در ui_ScanPage.c قرار شد ابتدا همه آیکون‌های mode مخفی شوند، بعد فقط آیکون‌های مربوط به mode فعلی نمایش داده شوند.
برای نمایش run state هم قرار شد ابتدا هر دو آیکون ui_play و ui_stop مخفی شوند و بعد فقط یکی بر اساس sub-state نمایش داده شود.
مشخص شد در پروژه شما نام/کارکرد آیکون‌های play و stop برعکس برداشت اولیه ما بوده؛ با تست دیدیم هنگام ورود به صفحه اسکن ui_stop نمایش داده می‌شود و این فعلاً مطابق منطق کد فعلی است.
در منطق ناوبری PAGE_SCAN_PAGE هم به این نتیجه رسیدیم:
ورود به صفحه اسکن با state اولیه RUNNING
بک اول: رفتن به IDLE
بک دوم: بازگشت به منوی اسکن‌ها