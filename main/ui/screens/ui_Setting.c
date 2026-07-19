// This file was generated/assembled for MAGI_ESP
// Target: ESP32-C3 + ESP-IDF + LVGL 9.3 + SquareLine Studio 1.6.0

#include "../ui.h"
#include "ui_Setting.h"
#include <stdbool.h>
#include <stdint.h>

/* ========================= Config ========================= */

#define SETTING_ITEM_COUNT          10
#define SETTING_CENTER_Y            0
#define SETTING_ITEM_WIDTH          191
#define SETTING_ITEM_HEIGHT         30
#define SETTING_ITEM_STEP_Y         40
#define SETTING_ITEM_GAP        5
#define SETTING_ITEM_STEP_Y     (SETTING_ITEM_HEIGHT + SETTING_ITEM_GAP)
#define SETTING_ANIM_TIME_MS    100
#define SETTING_ITEM_COLOR_NORMAL    lv_color_hex(0x3B82F6)
#define SETTING_ITEM_COLOR_SELECTED  lv_color_hex(0x2563EB)
#define SETTING_SELECTED_W   191
#define SETTING_SELECTED_H   30
#define SETTING_SIDE_W       165
#define SETTING_SIDE_H       24


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

typedef struct {
    lv_obj_t * leaving_item;
} setting_anim_done_data_t;

/* ========================= Internal State ========================= */

static lv_obj_t *setting_items[SETTING_ITEM_COUNT];
static int setting_selected_index = 0;
static bool setting_first_layout = true;
static bool setting_animating = false;
static int prev_visible[3] = { -1, -1, -1 };   // top, center, bottom

/* ========================= Helpers ========================= */

static int setting_wrap_index(int index)
{
    int res = index % SETTING_ITEM_COUNT;
    if(res < 0) res += SETTING_ITEM_COUNT;
    return res;
}

static void setting_prepare_visible_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_width(obj, SETTING_ITEM_WIDTH);
    lv_obj_set_height(obj, SETTING_ITEM_HEIGHT);
    lv_obj_set_y(obj, y);
}

static void setting_prepare_entering_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_width(obj, 0);
    lv_obj_set_height(obj, 0);
    lv_obj_set_y(obj, y);
}

static void setting_prepare_leaving_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_width(obj, SETTING_ITEM_WIDTH);
    lv_obj_set_height(obj, SETTING_ITEM_HEIGHT);
    lv_obj_set_y(obj, y);
}

lv_anim_t * moveUp_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!PropertyAnimation_0_user_data) return NULL;
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_duration(&PropertyAnimation_0, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 0, -SETTING_ITEM_STEP_Y);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y);
    
    return lv_anim_start(&PropertyAnimation_0);
}

lv_anim_t * fadeout_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!PropertyAnimation_0_user_data) return NULL;
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_duration(&PropertyAnimation_0, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_width);
    lv_anim_set_values(&PropertyAnimation_0, SETTING_ITEM_WIDTH, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_start(&PropertyAnimation_0);

    ui_anim_user_data_t * PropertyAnimation_1_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!PropertyAnimation_1_user_data) return NULL;
    PropertyAnimation_1_user_data->target = TargetObject;
    PropertyAnimation_1_user_data->val = -1;
    
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_duration(&PropertyAnimation_1, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_height);
    lv_anim_set_values(&PropertyAnimation_1, SETTING_ITEM_HEIGHT, 1);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay);
    lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
    
    return lv_anim_start(&PropertyAnimation_1);
}

lv_anim_t * fadein_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!PropertyAnimation_0_user_data) return NULL;
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_duration(&PropertyAnimation_0, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_height);
    lv_anim_set_values(&PropertyAnimation_0, 0, SETTING_SIDE_H);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_start(&PropertyAnimation_0);

    ui_anim_user_data_t * PropertyAnimation_1_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!PropertyAnimation_1_user_data) return NULL;
    PropertyAnimation_1_user_data->target = TargetObject;
    PropertyAnimation_1_user_data->val = -1;
    
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_duration(&PropertyAnimation_1, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_width);
    lv_anim_set_values(&PropertyAnimation_1, 0, SETTING_SIDE_W);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay);
    lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
    
    return lv_anim_start(&PropertyAnimation_1);
}

lv_anim_t * moveDown_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!user_data) return NULL;
    user_data->target = TargetObject;
    user_data->val = -1;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_duration(&a, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&a, user_data);
    lv_anim_set_custom_exec_cb(&a, _ui_anim_callback_set_y);
    lv_anim_set_values(&a, 0, SETTING_ITEM_STEP_Y);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_deleted_cb(&a, _ui_anim_callback_free_user_data);
    lv_anim_set_get_value_cb(&a, &_ui_anim_callback_get_y);

    return lv_anim_start(&a);
}

static void setting_transition_done_timer_cb(lv_timer_t *timer)
{
    setting_anim_done_data_t *data = (setting_anim_done_data_t *)lv_timer_get_user_data(timer);
    if(data != NULL) {
        if(data->leaving_item != NULL) {
            lv_obj_add_flag(data->leaving_item, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_width(data->leaving_item, SETTING_ITEM_WIDTH);
            lv_obj_set_height(data->leaving_item, SETTING_ITEM_HEIGHT);
        }
        lv_free(data);
    }
    setting_animating = false;
    lv_timer_delete(timer);
}

static void setting_transition_down(int leaving_idx, int moving1_idx, int moving2_idx, int entering_idx)
{
    lv_obj_t *leaving  = setting_items[leaving_idx];
    lv_obj_t *moving1  = setting_items[moving1_idx];
    lv_obj_t *moving2  = setting_items[moving2_idx];
    lv_obj_t *entering = setting_items[entering_idx];

    setting_prepare_leaving_item(leaving, -SETTING_ITEM_STEP_Y);
    setting_prepare_visible_item(moving1, 0);
    setting_prepare_visible_item(moving2, SETTING_ITEM_STEP_Y);
    setting_prepare_entering_item(entering, SETTING_ITEM_STEP_Y);

    moveUp_Animation(moving1, 0);
    moveUp_Animation(moving2, 0);
    fadeout_Animation(leaving, 0);
    fadein_Animation(entering, 0);

    lv_obj_move_foreground(moving1);

    setting_anim_done_data_t *done = lv_malloc(sizeof(setting_anim_done_data_t));
    if(done != NULL) {
        done->leaving_item = leaving;
        lv_timer_t *timer = lv_timer_create(setting_transition_done_timer_cb, SETTING_ANIM_TIME_MS + 20, done);
        lv_timer_set_repeat_count(timer, 1);
    } else {
        setting_animating = false;
    }
}

static void setting_transition_up(int entering_idx, int moving1_idx, int moving2_idx, int leaving_idx)
{
    lv_obj_t *entering = setting_items[entering_idx];
    lv_obj_t *moving1  = setting_items[moving1_idx];
    lv_obj_t *moving2  = setting_items[moving2_idx];
    lv_obj_t *leaving  = setting_items[leaving_idx];

    setting_prepare_entering_item(entering, -SETTING_ITEM_STEP_Y);
    setting_prepare_visible_item(moving1, -SETTING_ITEM_STEP_Y);
    setting_prepare_visible_item(moving2, 0);
    setting_prepare_leaving_item(leaving, SETTING_ITEM_STEP_Y);

    moveDown_Animation(moving1, 0);
    moveDown_Animation(moving2, 0);
    fadeout_Animation(leaving, 0);
    fadein_Animation(entering, 0);

    lv_obj_move_foreground(moving2);

    setting_anim_done_data_t *done = lv_malloc(sizeof(setting_anim_done_data_t));
    if(done != NULL) {
        done->leaving_item = leaving;
        lv_timer_t *timer = lv_timer_create(setting_transition_done_timer_cb, SETTING_ANIM_TIME_MS + 20, done);
        lv_timer_set_repeat_count(timer, 1);
    } else {
        setting_animating = false;
    }
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

static void setting_store_current_triplet(int top, int center, int bottom)
{
    prev_visible[0] = top;
    prev_visible[1] = center;
    prev_visible[2] = bottom;
}

static void setting_transition_to(int new_selected_index)
{
    const int old_selected = setting_selected_index;
    const bool moving_down = (setting_wrap_index(old_selected + 1) == new_selected_index);
    const bool moving_up   = (setting_wrap_index(old_selected - 1) == new_selected_index);

    const int new_top    = setting_wrap_index(new_selected_index - 1);
    const int new_center = new_selected_index;
    const int new_bottom = setting_wrap_index(new_selected_index + 1);

    if(setting_first_layout || prev_visible[0] < 0 || prev_visible[1] < 0 || prev_visible[2] < 0) {
        for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
            if(setting_items[i]) {
                lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_width(setting_items[i], SETTING_ITEM_WIDTH);
                lv_obj_set_height(setting_items[i], SETTING_ITEM_HEIGHT);
            }
        }
        setting_prepare_visible_item(setting_items[new_top], -SETTING_ITEM_STEP_Y);
        setting_prepare_visible_item(setting_items[new_center], 0);
        setting_prepare_visible_item(setting_items[new_bottom], SETTING_ITEM_STEP_Y);
        lv_obj_move_foreground(setting_items[new_center]);

        setting_store_current_triplet(new_top, new_center, new_bottom);
        setting_selected_index = new_selected_index;
        setting_first_layout = false;
        setting_animating = false;
        return;
    }

    if(setting_animating) return;
    setting_animating = true;

    if(moving_down) {
        setting_transition_down(prev_visible[0], prev_visible[1], prev_visible[2], new_bottom);
    } else if(moving_up) {
        setting_transition_up(new_top, prev_visible[0], prev_visible[1], prev_visible[2]);
    } else {
        for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
            if(setting_items[i]) {
                lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_width(setting_items[i], SETTING_ITEM_WIDTH);
                lv_obj_set_height(setting_items[i], SETTING_ITEM_HEIGHT);
            }
        }
        setting_prepare_visible_item(setting_items[new_top], -SETTING_ITEM_STEP_Y);
        setting_prepare_visible_item(setting_items[new_center], 0);
        setting_prepare_visible_item(setting_items[new_bottom], SETTING_ITEM_STEP_Y);
        lv_obj_move_foreground(setting_items[new_center]);
        setting_animating = false; // آزاد کردن برای حالت پرش
    }

    setting_store_current_triplet(new_top, new_center, new_bottom);
    setting_selected_index = new_selected_index;
}
static bool setting_is_visible_index(int index)
{
    return index == prev_visible[0] ||
           index == prev_visible[1] ||
           index == prev_visible[2];
}
static void setting_update_visual_state(void)
{
    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        lv_obj_t *item = setting_items[i];
        if(item == NULL) continue;

        if(i == setting_selected_index) {
            lv_obj_set_style_bg_color(
                item,
                SETTING_ITEM_COLOR_SELECTED,
                LV_PART_MAIN | LV_STATE_DEFAULT
            );
            lv_obj_set_style_bg_opa(
                item,
                LV_OPA_COVER,
                LV_PART_MAIN | LV_STATE_DEFAULT
            );

            lv_obj_set_width(item, SETTING_SELECTED_W);
            lv_obj_set_height(item, SETTING_SELECTED_H);
        }
        else if(setting_is_visible_index(i)) {
            lv_obj_set_style_bg_color(
                item,
                SETTING_ITEM_COLOR_NORMAL,
                LV_PART_MAIN | LV_STATE_DEFAULT
            );
            lv_obj_set_style_bg_opa(
                item,
                LV_OPA_COVER,
                LV_PART_MAIN | LV_STATE_DEFAULT
            );

            lv_obj_set_width(item, SETTING_SIDE_W);
            lv_obj_set_height(item, SETTING_SIDE_H);
        }
    }
}

// static void setting_update_visual_state(void)
// {
//     for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
//         if(setting_items[i] == NULL) continue;
//         if(i == setting_selected_index) {
//             lv_obj_set_style_bg_color(setting_items[i], lv_color_hex(0x3B82F6), LV_PART_MAIN | LV_STATE_DEFAULT);
//         } else {
//             lv_obj_set_style_bg_color(setting_items[i], lv_color_hex(0x2B2B2B), LV_PART_MAIN | LV_STATE_DEFAULT);
//         }
//     }
// }

/* ========================= Public API ========================= */

void ui_Setting_update_view(int focus_idx)
{
    int wrapped = setting_wrap_index(focus_idx);
    if(wrapped == setting_selected_index && !setting_first_layout) return;
    setting_transition_to(wrapped);
    setting_update_visual_state();
}

bool ui_Setting_is_ready(void)
{
    return (ui_Setting != NULL) && (ui_AutoCal != NULL);
}

void ui_Setting_screen_init(void)
{
    ui_Setting = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Setting, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Setting, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Setting, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_AutoCal = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_AutoCal, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_AutoCal, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_AutoCal, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_AutoCal, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblAutoCal = lv_label_create(ui_AutoCal);
    lv_obj_set_align(ui_LblAutoCal, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCal, "کالیبره خودکار");
    lv_obj_set_style_text_font(ui_LblAutoCal, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_AutoCalPls = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_AutoCalPls, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_AutoCalPls, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_AutoCalPls, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_AutoCalPls, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblAutoCalPls = lv_label_create(ui_AutoCalPls);
    lv_obj_set_align(ui_LblAutoCalPls, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalPls, "تعداد پالس کالیبره");
    lv_obj_set_style_text_font(ui_LblAutoCalPls, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_PulsMax = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_PulsMax, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_PulsMax, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_PulsMax, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_PulsMax, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblPalsMax = lv_label_create(ui_PulsMax);
    lv_obj_set_align(ui_LblPalsMax, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPalsMax, "حداکثر پالس");
    lv_obj_set_style_text_font(ui_LblPalsMax, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_DelayTime = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_DelayTime, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_DelayTime, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_DelayTime, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_DelayTime, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblDelayTime = lv_label_create(ui_DelayTime);
    lv_obj_set_align(ui_LblDelayTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblDelayTime, "زمان تأخیر");
    lv_obj_set_style_text_font(ui_LblDelayTime, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_StopTrg = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_StopTrg, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_StopTrg, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_StopTrg, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_StopTrg, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblStopTrg = lv_label_create(ui_StopTrg);
    lv_obj_set_align(ui_LblStopTrg, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblStopTrg, "تریگر توقف");
    lv_obj_set_style_text_font(ui_LblStopTrg, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Beep = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_Beep, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_Beep, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_Beep, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_Beep, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBeep = lv_label_create(ui_Beep);
    lv_obj_set_align(ui_LblBeep, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBeep, "بوق");
    lv_obj_set_style_text_font(ui_LblBeep, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlAutoOff = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlAutoOff, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_BlAutoOff, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_BlAutoOff, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlAutoOff, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlAutoOff = lv_label_create(ui_BlAutoOff);
    lv_obj_set_align(ui_LblBlAutoOff, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlAutoOff, "خاموشی خودکار بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlAutoOff, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlAutoConnect = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlAutoConnect, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_BlAutoConnect, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_BlAutoConnect, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlAutoConnect, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlAutoConnect = lv_label_create(ui_BlAutoConnect);
    lv_obj_set_align(ui_LblBlAutoConnect, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlAutoConnect, "اتصال خودکار بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlAutoConnect, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlPass = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlPass, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_BlPass, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_BlPass, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlPass, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlPass = lv_label_create(ui_BlPass);
    lv_obj_set_align(ui_LblBlPass, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlPass, "رمز بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlPass, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlName = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlName, SETTING_ITEM_WIDTH);
    lv_obj_set_height(ui_BlName, SETTING_ITEM_HEIGHT);
    lv_obj_set_align(ui_BlName, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlName, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlName = lv_label_create(ui_BlName);
    lv_obj_set_align(ui_LblBlName, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlName, "نام بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlName, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    setting_init_item_array();

    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        if(setting_items[i]) {
            lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
           // setting_set_zoom(setting_items[i], SETTING_ZOOM_NORMAL);
        }
    }

    prev_visible[0] = -1;
    prev_visible[1] = -1;
    prev_visible[2] = -1;

    setting_selected_index = 0;
    setting_first_layout = true;
    setting_animating = false;

    ui_Setting_update_view(0);
}

void ui_Setting_screen_destroy(void)
{
    setting_animating = false; // جلوگیری از تداخل تایمرها بعد از حذف
    if(ui_Setting != NULL) {
        lv_obj_del(ui_Setting);
        ui_Setting = NULL;
    }
}
