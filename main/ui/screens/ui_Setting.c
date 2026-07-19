// This file was generated/assembled for MAGI_ESP
// Target: ESP32-C3 + ESP-IDF + LVGL 9.3 + SquareLine Studio 1.6.0

#include "../ui.h"
#include "ui_Setting.h"
#include <stdbool.h>
#include <stdint.h>

/* ========================= Config ========================= */

#define SETTING_ITEM_COUNT          10
#define SETTING_CENTER_Y            0
#define SETTING_ITEM_GAP            42
#define SETTING_ANIM_TIME_MS        220

/* ========================= Animation Data ========================= */

typedef struct {
    lv_obj_t *obj;
    int32_t start_y;
    int32_t end_y;
} setting_anim_data_t;

/* ========================= UI Objects ========================= */

lv_obj_t * ui_Setting = NULL;

lv_obj_t * ui_AutoCal = NULL;
lv_obj_t * ui_LblAutoCal = NULL;

lv_obj_t * ui_AutoCalPls = NULL;
lv_obj_t * ui_LblAutoCalPls = NULL;

lv_obj_t * ui_PulsMax = NULL;
lv_obj_t * ui_LblPalsMax = NULL;

lv_obj_t * ui_DelayTime = NULL;
lv_obj_t * ui_LblDelayTime = NULL;

lv_obj_t * ui_StopTrg = NULL;
lv_obj_t * ui_LblStopTrg = NULL;

lv_obj_t * ui_Beep = NULL;
lv_obj_t * ui_LblBeep = NULL;

lv_obj_t * ui_BlAutoOff = NULL;
lv_obj_t * ui_LblBlAutoOff = NULL;

lv_obj_t * ui_BlAutoConnect = NULL;
lv_obj_t * ui_LblBlAutoConnect = NULL;

lv_obj_t * ui_BlPass = NULL;
lv_obj_t * ui_LblBlPass = NULL;

lv_obj_t * ui_BlName = NULL;
lv_obj_t * ui_LblBlName = NULL;

/* ========================= Internal State ========================= */

static lv_obj_t *setting_items[SETTING_ITEM_COUNT];
static setting_anim_data_t setting_anim_data[SETTING_ITEM_COUNT];
static int setting_selected_index = 0;
static bool setting_first_layout = true;

/* ========================= Helpers ========================= */

static int setting_wrap_index(int index)
{
    while(index < 0) {
        index += SETTING_ITEM_COUNT;
    }
    while(index >= SETTING_ITEM_COUNT) {
        index -= SETTING_ITEM_COUNT;
    }
    return index;
}

static void setting_y_anim_cb(void *var, int32_t val)
{
    setting_anim_data_t *data = (setting_anim_data_t *)var;
    if(data == NULL || data->obj == NULL) return;

    int32_t cur_y = data->start_y + (((data->end_y - data->start_y) * val) / 1000);
    lv_obj_set_y(data->obj, cur_y);
}

static void setting_hide_all_items(void)
{
    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        if(setting_items[i] != NULL) {
            lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void setting_show_item(lv_obj_t *obj)
{
    if(obj != NULL) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void setting_place_item_immediate(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_set_align(obj, LV_ALIGN_CENTER);
    lv_obj_set_y(obj, y);
}

static void setting_animate_item_to_y(int item_index, int32_t target_y)
{
    lv_obj_t *obj = setting_items[item_index];
    if(obj == NULL) return;

    setting_anim_data[item_index].obj = obj;
    setting_anim_data[item_index].start_y = lv_obj_get_y(obj);
    setting_anim_data[item_index].end_y = target_y;

    lv_anim_delete(&setting_anim_data[item_index], setting_y_anim_cb);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &setting_anim_data[item_index]);
    lv_anim_set_values(&a, 0, 1000);
    lv_anim_set_duration(&a, SETTING_ANIM_TIME_MS);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&a, setting_y_anim_cb);
    lv_anim_start(&a);
}

static void setting_init_item_array(void)
{
    setting_items[0] = ui_AutoCal;
    setting_items[1] = ui_AutoCalPls;
    setting_items[2] = ui_PulsMax;
    setting_items[3] = ui_DelayTime;
    setting_items[4] = ui_StopTrg;
    setting_items[5] = ui_Beep;
    setting_items[6] = ui_BlAutoOff;
    setting_items[7] = ui_BlAutoConnect;
    setting_items[8] = ui_BlPass;
    setting_items[9] = ui_BlName;
}

static void setting_render_positions(void)
{
    const int prev_index = setting_wrap_index(setting_selected_index - 1);
    const int curr_index = setting_wrap_index(setting_selected_index);
    const int next_index = setting_wrap_index(setting_selected_index + 1);

    const int32_t y_top    = SETTING_CENTER_Y - SETTING_ITEM_GAP;
    const int32_t y_center = SETTING_CENTER_Y;
    const int32_t y_bottom = SETTING_CENTER_Y + SETTING_ITEM_GAP;

    setting_hide_all_items();

    setting_show_item(setting_items[prev_index]);
    setting_show_item(setting_items[curr_index]);
    setting_show_item(setting_items[next_index]);

    if(setting_items[curr_index] != NULL) {
        lv_obj_move_foreground(setting_items[curr_index]);
    }

    if(setting_first_layout) {
        setting_place_item_immediate(setting_items[prev_index], y_top);
        setting_place_item_immediate(setting_items[curr_index], y_center);
        setting_place_item_immediate(setting_items[next_index], y_bottom);
        setting_first_layout = false;
        return;
    }

    setting_animate_item_to_y(prev_index, y_top);
    setting_animate_item_to_y(curr_index, y_center);
    setting_animate_item_to_y(next_index, y_bottom);
}

/* ========================= Public API ========================= */

void ui_Setting_update_view(int focus_idx)
{
    setting_selected_index = setting_wrap_index(focus_idx);
    setting_render_positions();
}

bool ui_Setting_is_ready(void)
{
    return (ui_Setting != NULL) &&
           (ui_AutoCal != NULL) &&
           (ui_LblAutoCal != NULL) &&
           (ui_AutoCalPls != NULL) &&
           (ui_LblAutoCalPls != NULL) &&
           (ui_PulsMax != NULL) &&
           (ui_LblPalsMax != NULL) &&
           (ui_DelayTime != NULL) &&
           (ui_LblDelayTime != NULL) &&
           (ui_StopTrg != NULL) &&
           (ui_LblStopTrg != NULL) &&
           (ui_Beep != NULL) &&
           (ui_LblBeep != NULL) &&
           (ui_BlAutoOff != NULL) &&
           (ui_LblBlAutoOff != NULL) &&
           (ui_BlAutoConnect != NULL) &&
           (ui_LblBlAutoConnect != NULL) &&
           (ui_BlPass != NULL) &&
           (ui_LblBlPass != NULL) &&
           (ui_BlName != NULL) &&
           (ui_LblBlName != NULL);
}

void ui_Setting_screen_init(void)
{
    ui_Setting = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Setting, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Setting, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Setting, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 0 ========================= */
    ui_AutoCal = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_AutoCal, 191);
    lv_obj_set_height(ui_AutoCal, 30);
    lv_obj_set_align(ui_AutoCal, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_AutoCal, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_AutoCal, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblAutoCal = lv_label_create(ui_AutoCal);
    lv_obj_set_width(ui_LblAutoCal, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblAutoCal, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblAutoCal, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCal, "کالیبره خودکار");
    lv_obj_set_style_text_font(ui_LblAutoCal, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 1 ========================= */
    ui_AutoCalPls = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_AutoCalPls, 191);
    lv_obj_set_height(ui_AutoCalPls, 30);
    lv_obj_set_align(ui_AutoCalPls, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_AutoCalPls, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_AutoCalPls, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblAutoCalPls = lv_label_create(ui_AutoCalPls);
    lv_obj_set_width(ui_LblAutoCalPls, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblAutoCalPls, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblAutoCalPls, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalPls, "تعداد پالس کالیبره");
    lv_obj_set_style_text_font(ui_LblAutoCalPls, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 2 ========================= */
    ui_PulsMax = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_PulsMax, 191);
    lv_obj_set_height(ui_PulsMax, 30);
    lv_obj_set_align(ui_PulsMax, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_PulsMax, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_PulsMax, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblPalsMax = lv_label_create(ui_PulsMax);
    lv_obj_set_width(ui_LblPalsMax, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblPalsMax, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblPalsMax, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPalsMax, "حداکثر پالس");
    lv_obj_set_style_text_font(ui_LblPalsMax, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 3 ========================= */
    ui_DelayTime = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_DelayTime, 191);
    lv_obj_set_height(ui_DelayTime, 30);
    lv_obj_set_align(ui_DelayTime, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_DelayTime, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_DelayTime, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblDelayTime = lv_label_create(ui_DelayTime);
    lv_obj_set_width(ui_LblDelayTime, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblDelayTime, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblDelayTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblDelayTime, "زمان تأخیر");
    lv_obj_set_style_text_font(ui_LblDelayTime, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 4 ========================= */
    ui_StopTrg = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_StopTrg, 191);
    lv_obj_set_height(ui_StopTrg, 30);
    lv_obj_set_align(ui_StopTrg, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_StopTrg, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_StopTrg, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblStopTrg = lv_label_create(ui_StopTrg);
    lv_obj_set_width(ui_LblStopTrg, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblStopTrg, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblStopTrg, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblStopTrg, "تریگر توقف");
    lv_obj_set_style_text_font(ui_LblStopTrg, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 5 ========================= */
    ui_Beep = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_Beep, 191);
    lv_obj_set_height(ui_Beep, 30);
    lv_obj_set_align(ui_Beep, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Beep, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_Beep, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblBeep = lv_label_create(ui_Beep);
    lv_obj_set_width(ui_LblBeep, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblBeep, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblBeep, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBeep, "بوق");
    lv_obj_set_style_text_font(ui_LblBeep, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 6 ========================= */
    ui_BlAutoOff = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlAutoOff, 191);
    lv_obj_set_height(ui_BlAutoOff, 30);
    lv_obj_set_align(ui_BlAutoOff, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BlAutoOff, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BlAutoOff, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblBlAutoOff = lv_label_create(ui_BlAutoOff);
    lv_obj_set_width(ui_LblBlAutoOff, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblBlAutoOff, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblBlAutoOff, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlAutoOff, "خاموشی خودکار بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlAutoOff, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 7 ========================= */
    ui_BlAutoConnect = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlAutoConnect, 191);
    lv_obj_set_height(ui_BlAutoConnect, 30);
    lv_obj_set_align(ui_BlAutoConnect, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BlAutoConnect, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BlAutoConnect, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblBlAutoConnect = lv_label_create(ui_BlAutoConnect);
    lv_obj_set_width(ui_LblBlAutoConnect, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblBlAutoConnect, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblBlAutoConnect, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlAutoConnect, "اتصال خودکار بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlAutoConnect, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 8 ========================= */
    ui_BlPass = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlPass, 191);
    lv_obj_set_height(ui_BlPass, 30);
    lv_obj_set_align(ui_BlPass, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BlPass, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BlPass, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblBlPass = lv_label_create(ui_BlPass);
    lv_obj_set_width(ui_LblBlPass, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblBlPass, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblBlPass, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlPass, "رمز بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlPass, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========================= Item 9 ========================= */
    ui_BlName = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlName, 191);
    lv_obj_set_height(ui_BlName, 30);
    lv_obj_set_align(ui_BlName, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BlName, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BlName, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblBlName = lv_label_create(ui_BlName);
    lv_obj_set_width(ui_LblBlName, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblBlName, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblBlName, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlName, "نام بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlName, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    setting_init_item_array();

    setting_selected_index = 0;
    setting_first_layout = true;
    setting_render_positions();
}

void ui_Setting_screen_destroy(void)
{
    if(ui_Setting != NULL) {
        lv_obj_del(ui_Setting);
    }

    ui_Setting = NULL;

    ui_AutoCal = NULL;
    ui_LblAutoCal = NULL;

    ui_AutoCalPls = NULL;
    ui_LblAutoCalPls = NULL;

    ui_PulsMax = NULL;
    ui_LblPalsMax = NULL;

    ui_DelayTime = NULL;
    ui_LblDelayTime = NULL;

    ui_StopTrg = NULL;
    ui_LblStopTrg = NULL;

    ui_Beep = NULL;
    ui_LblBeep = NULL;

    ui_BlAutoOff = NULL;
    ui_LblBlAutoOff = NULL;

    ui_BlAutoConnect = NULL;
    ui_LblBlAutoConnect = NULL;

    ui_BlPass = NULL;
    ui_LblBlPass = NULL;

    ui_BlName = NULL;
    ui_LblBlName = NULL;
}
