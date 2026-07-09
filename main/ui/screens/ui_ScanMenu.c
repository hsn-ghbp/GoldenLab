#include "../ui.h"
#include <stdbool.h>
#include <stdint.h>

#define SCAN_ITEM_COUNT 4

typedef struct {
    lv_obj_t *obj;
    int32_t start_x;
    int32_t start_y;
    int32_t end_x;
    int32_t end_y;
    int32_t start_scale;
    int32_t end_scale;
} scan_anim_data_t;

static lv_obj_t *scan_icons[SCAN_ITEM_COUNT];
static int scan_selected = 0;
static bool first_layout = true;

lv_obj_t *ui_ScanMenu = NULL;
lv_obj_t *ui_manpc = NULL;
lv_obj_t *ui_autopc = NULL;
lv_obj_t *ui_automem = NULL;
lv_obj_t *ui_manmem = NULL;

static void scan_update_positions(void);
static void scan_xy_anim_cb(void *var, int32_t val);
bool ui_ScanMenu_is_ready(void);

void ui_ScanMenu_screen_init(void)
{
     
    ui_ScanMenu = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_ScanMenu, LV_OBJ_FLAG_SCROLLABLE);

    ui_manpc = lv_image_create(ui_ScanMenu);
    lv_image_set_src(ui_manpc, &ui_img_manpc_png);

    ui_autopc = lv_image_create(ui_ScanMenu);
    lv_image_set_src(ui_autopc, &ui_img_autopc_png);

    ui_automem = lv_image_create(ui_ScanMenu);
    lv_image_set_src(ui_automem, &ui_img_automem_png);

    ui_manmem = lv_image_create(ui_ScanMenu);
    lv_image_set_src(ui_manmem, &ui_img_manmem_png);

    scan_icons[0] = ui_manpc;
    scan_icons[1] = ui_autopc;
    scan_icons[2] = ui_automem;
    scan_icons[3] = ui_manmem;

    lv_obj_update_layout(ui_ScanMenu);

    first_layout = true;
    scan_selected = 0;
    scan_update_positions();
    
}

static void scan_xy_anim_cb(void *var, int32_t val)
{
    scan_anim_data_t *data = (scan_anim_data_t *)var;

    int32_t cur_x = data->start_x + ((data->end_x - data->start_x) * val) / 1000;
    int32_t cur_y = data->start_y + ((data->end_y - data->start_y) * val) / 1000;
    int32_t cur_scale = data->start_scale + ((data->end_scale - data->start_scale) * val) / 1000;

    lv_obj_set_pos(data->obj, cur_x, cur_y);

    lv_image_set_pivot(
        data->obj,
        lv_obj_get_width(data->obj) / 2,
        lv_obj_get_height(data->obj) / 2
    );
    lv_image_set_scale(data->obj, cur_scale);
}

static void scan_update_positions(void)
{
    /*
     * چینش 4 آیتم به صورت لوزی/دایره‌ای:
     *
     * pos 0 = بالا (فعال)
     * pos 1 = راست
     * pos 2 = پایین
     * pos 3 = چپ
     *
     * اعداد را اگر خواستی بعداً با UI واقعی‌ات فاین‌تیون کن.
     */

    static const int16_t target_positions_x[SCAN_ITEM_COUNT] = {
        120 - 24,   // pos 0: بالا
        180 - 24,   // pos 1: راست
        120 - 24,   // pos 2: پایین
        60  - 24    // pos 3: چپ
    };

    static const int16_t target_positions_y[SCAN_ITEM_COUNT] = {
        50  - 24,   // pos 0: بالا
        120 - 24,   // pos 1: راست
        190 - 24,   // pos 2: پایین
        120 - 24    // pos 3: چپ
    };

    static scan_anim_data_t anim_data[SCAN_ITEM_COUNT];

    for(int i = 0; i < SCAN_ITEM_COUNT; i++)
    {
        int pos = (i - scan_selected + SCAN_ITEM_COUNT) % SCAN_ITEM_COUNT;

        int32_t target_x = target_positions_x[pos];
        int32_t target_y = target_positions_y[pos];

        // آیتم فعال بزرگتر
        int32_t target_scale = (pos == 0) ? 320 : 220;

        // کمی افست برای تأکید روی آیتم فعال
        if(pos == 0) {
            target_y += 10;
        }

        lv_anim_delete(&anim_data[i], scan_xy_anim_cb);

        if(first_layout) {
            lv_obj_set_pos(scan_icons[i], target_x, target_y);
            lv_image_set_pivot(
                scan_icons[i],
                lv_obj_get_width(scan_icons[i]) / 2,
                lv_obj_get_height(scan_icons[i]) / 2
            );
            lv_image_set_scale(scan_icons[i], target_scale);
            continue;
        }

        anim_data[i].obj = scan_icons[i];
        anim_data[i].start_x = lv_obj_get_x(scan_icons[i]);
        anim_data[i].start_y = lv_obj_get_y(scan_icons[i]);
        anim_data[i].end_x = target_x;
        anim_data[i].end_y = target_y;
        anim_data[i].start_scale = lv_image_get_scale(scan_icons[i]);
        anim_data[i].end_scale = target_scale;

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, &anim_data[i]);
        lv_anim_set_values(&a, 0, 1000);
        lv_anim_set_duration(&a, 350);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
        lv_anim_set_exec_cb(&a, scan_xy_anim_cb);
        lv_anim_start(&a);
    }

    first_layout = false;
}

/**
 * @brief فقط ایندکس فوکوس شده را از بیرون می‌گیرد و UI را آپدیت می‌کند
 * @param index شماره آیتم فوکوس شده (0 تا 3)
 */
void scan_set_focused_index(int index)
{
    if(index < 0 || index >= SCAN_ITEM_COUNT) return;

    if(scan_selected == index && !first_layout) return;

    scan_selected = index;
    scan_update_positions();
}


#include <stdbool.h>

bool ui_ScanMenu_is_ready(void)
{
    return (ui_ScanMenu != NULL) &&
           (ui_manpc != NULL) &&
           (ui_autopc != NULL) &&
           (ui_automem != NULL) &&
           (ui_manmem != NULL);
}


void ui_ScanMenu_screen_destroy(void)
{
    for(int i = 0; i < SCAN_ITEM_COUNT; i++) {
        lv_anim_delete(scan_icons[i], NULL);
    }

    if(ui_ScanMenu)
        lv_obj_del(ui_ScanMenu);

    ui_ScanMenu = NULL;
    ui_manpc = NULL;
    ui_autopc = NULL;
    ui_automem = NULL;
    ui_manmem = NULL;
}
