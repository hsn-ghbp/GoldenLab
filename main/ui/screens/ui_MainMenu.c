#include "../ui.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define MENU_ITEM_COUNT 5
#define MENU_CENTER_X   120
#define MENU_CENTER_Y   120
#define MENU_RADIUS     75
#define ICON_SIZE       48

typedef struct {
    lv_obj_t *obj;
    int32_t start_x;
    int32_t start_y;
    int32_t end_x;
    int32_t end_y;
    int32_t start_scale;
    int32_t end_scale;
} menu_anim_data_t;

static lv_obj_t *menu_icons[MENU_ITEM_COUNT];
static int menu_selected = 0;
static bool first_layout = true;

lv_obj_t * ui_MainMenu = NULL;
lv_obj_t * ui_ScanIcon = NULL;
lv_obj_t * ui_SendDataIcon = NULL;
lv_obj_t * ui_MemoryIcon = NULL;
lv_obj_t * ui_SettingIcon = NULL;
lv_obj_t * ui_InfoIcon = NULL;

static void menu_update_positions(void);
static void menu_xy_anim_cb(void *var, int32_t v);

void ui_MainMenu_screen_init(void)
{
    ui_MainMenu = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_MainMenu, LV_OBJ_FLAG_SCROLLABLE);

    ui_ScanIcon = lv_image_create(ui_MainMenu);
    lv_image_set_src(ui_ScanIcon, &ui_img_scan_png);

    ui_SendDataIcon = lv_image_create(ui_MainMenu);
    lv_image_set_src(ui_SendDataIcon, &ui_img_send_png);

    ui_MemoryIcon = lv_image_create(ui_MainMenu);
    lv_image_set_src(ui_MemoryIcon, &ui_img_memory_png);

    ui_SettingIcon = lv_image_create(ui_MainMenu);
    lv_image_set_src(ui_SettingIcon, &ui_img_setting_png);

    ui_InfoIcon = lv_image_create(ui_MainMenu);
    lv_image_set_src(ui_InfoIcon, &ui_img_about_png);

    menu_icons[0] = ui_ScanIcon;
    menu_icons[1] = ui_SendDataIcon;
    menu_icons[2] = ui_MemoryIcon;
    menu_icons[3] = ui_SettingIcon;
    menu_icons[4] = ui_InfoIcon;

    lv_obj_update_layout(ui_MainMenu);

    first_layout = true;
    menu_update_positions();
}


static void menu_xy_anim_cb(void *var, int32_t val)
{
    menu_anim_data_t *data = (menu_anim_data_t *)var;
    
    // محاسبات انتقال موقعیت و مقیاس بر اساس درصدی از حرکت (0 تا 1000)
    int32_t cur_x = data->start_x + ((data->end_x - data->start_x) * val) / 1000;
    int32_t cur_y = data->start_y + ((data->end_y - data->start_y) * val) / 1000;
    int32_t cur_scale = data->start_scale + ((data->end_scale - data->start_scale) * val) / 1000;

    lv_obj_set_pos(data->obj, cur_x, cur_y);
    
    // تنظیم نقطه ثقل تغییر مقیاس به مرکز تصویر
    lv_image_set_pivot(data->obj, lv_obj_get_width(data->obj) / 2, lv_obj_get_height(data->obj) / 2);
    lv_image_set_scale(data->obj, cur_scale);
}

static void menu_update_positions(void)
{
    // مختصات‌های X و Y پیش‌محاسبه شده برای ۵ موقعیت روی دایره
    static const int16_t target_positions_x[MENU_ITEM_COUNT] = {
        120 - 24,         // موقعیت 0 (بالا - آیتم فعال)
        191 - 24,         // موقعیت 1 (راست-بالا)
        164 - 24,         // موقعیت 2 (راست-پایین)
        76 - 24,          // موقعیت 3 (چپ-پایین)
        49 - 24           // موقعیت 4 (چپ-بالا)
    };

    static const int16_t target_positions_y[MENU_ITEM_COUNT] = {
        45 - 24,          // موقعیت 0 (بالا - آیتم فعال)
        97 - 24,          // موقعیت 1 (راست-بالا)
        181 - 24,         // موقعیت 2 (راست-پایین)
        181 - 24,         // موقعیت 3 (چپ-پایین)
        97 - 24           // موقعیت 4 (چپ-بالا)
    };

    static menu_anim_data_t anim_data[MENU_ITEM_COUNT];

    for(int i = 0; i < MENU_ITEM_COUNT; i++)
    {
        // محاسبه موقعیت نسبی بر اساس آیتم انتخاب شده فعلی
        int pos = (i - menu_selected + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;

        int32_t target_x = target_positions_x[pos];
        int32_t target_y = target_positions_y[pos];
        
        // اگر موقعیت 0 باشد (فعال)، بزرگتر (320) و در غیر این صورت معمولی (220)
        int32_t target_scale = (pos == 0) ? 320 : 220; 

        if(pos == 0) {
            target_y += 10; // افست جزئی برای متمایز شدن آیتم وسط
        }

        // اصلاح باگ: حذف انیمیشن‌های در حال اجرا بر روی داده ساختار مربوطه
        lv_anim_delete(&anim_data[i], menu_xy_anim_cb);

        if(first_layout) {
            lv_obj_set_pos(menu_icons[i], target_x, target_y);
            lv_image_set_pivot(menu_icons[i], lv_obj_get_width(menu_icons[i]) / 2, lv_obj_get_height(menu_icons[i]) / 2);
            lv_image_set_scale(menu_icons[i], target_scale);
            continue;
        }

        // مقداردهی داده‌های انیمیشن ساختار استاتیک
        anim_data[i].obj = menu_icons[i];
        anim_data[i].start_x = lv_obj_get_x(menu_icons[i]);
        anim_data[i].start_y = lv_obj_get_y(menu_icons[i]);
        anim_data[i].end_x = target_x;
        anim_data[i].end_y = target_y;
        
        anim_data[i].start_scale = lv_image_get_scale(menu_icons[i]);
        anim_data[i].end_scale = target_scale;

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, &anim_data[i]);
        lv_anim_set_values(&a, 0, 1000);
        lv_anim_set_duration(&a, 350); // کاهش جزئی زمان برای پاسخ‌دهی سریع‌تر و نرم‌تر (350ms)
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
        lv_anim_set_exec_cb(&a, menu_xy_anim_cb);
        lv_anim_start(&a);
    }
    first_layout = false;
}

/**
 * @brief به روز رسانی مستقیم موقعیت منو صرفاً با داشتن ایندکس فوکوس شده
 * @param index شماره آیتم فوکوس شده (0 تا 4)
 */
void menu_set_focused_index(int index)
{
    if(index < 0 || index >= MENU_ITEM_COUNT) return;
    
    // اگر از قبل روی همین ایندکس بودیم و اولین بار نیست، کار اضافه‌ای انجام ندهیم
    if(menu_selected == index && !first_layout) return;

    menu_selected = index;
    menu_update_positions();
}

void ui_MainMenu_screen_destroy(void)
{
    // حذف تمام انیمیشن‌های معلق در صورت نابودی صفحه برای جلوگیری از نشت حافظه
    for(int i = 0; i < MENU_ITEM_COUNT; i++) {
        lv_anim_delete(menu_icons[i], NULL);
    }

    if(ui_MainMenu)
        lv_obj_del(ui_MainMenu);

    ui_MainMenu = NULL;
    ui_ScanIcon = NULL;
    ui_SendDataIcon = NULL;
    ui_MemoryIcon = NULL;
    ui_SettingIcon = NULL;
    ui_InfoIcon = NULL;
}
