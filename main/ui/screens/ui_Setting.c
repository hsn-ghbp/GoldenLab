// This file was generated/assembled for MAGI_ESP
// Target: ESP32-C3 + ESP-IDF + LVGL 9.3 + SquareLine Studio 1.6.0

#include "../ui.h"
#include "ui_Setting.h"
#include <stdbool.h>
#include <stdint.h>
#include "brain.h"

/* ========================= Config ========================= */

#define SETTING_ITEM_COUNT          10
#define SETTING_CENTER_Y            0

#define SETTING_ANIM_TIME_MS        100

#define SETTING_ITEM_GAP            5

#define SETTING_SELECTED_W          191
#define SETTING_SELECTED_H          30

#define SETTING_SIDE_W              165
#define SETTING_SIDE_H              24

#define SETTING_ITEM_COLOR_SELECTED lv_color_hex(0x2563EB)
#define SETTING_ITEM_COLOR_NORMAL   lv_color_hex(0x34C5FF)

#define SETTING_OK_MOVE_UP_Y        50
#define SETTING_OK_ANIM_TIME_MS     220

/* میزان شفافیت آیتم‌ها */
#define SETTING_SELECTED_OPA   LV_OPA_COVER
#define SETTING_SIDE_OPA       LV_OPA_50

/* میزان شفافیت متن‌ها برای آیتم انتخاب‌شده و کناری */
#define SETTING_TEXT_OPA_SELECTED LV_OPA_COVER
#define SETTING_TEXT_OPA_SIDE     LV_OPA_70

static int32_t setting_selected_closed_y = 0;
static int32_t setting_selected_open_y   = -60;
static int setting_restore_focus_idx = 0;
static void setting_anim_set_y_cb(void * var, int32_t v);

static void setting_hide_all_except(int focus_idx);
bool ui_Setting_focus_close_done(void);
static volatile bool setting_close_anim_running = false;
static volatile bool setting_open_anim_running = false;

/*
 * فاصله مرکز تا مرکز برای رسیدن به gap واقعی:
 * gap = side_y - (selected_h/2) - (side_h/2)
 */
#define SETTING_SIDE_OFFSET_Y \
    ((SETTING_SELECTED_H / 2) + SETTING_ITEM_GAP + (SETTING_SIDE_H / 2))

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
lv_obj_t * ui_LblAutoCalOff = NULL;
lv_obj_t * ui_SwAutoCal= NULL;
lv_obj_t * ui_LblAutoCalOn = NULL;
lv_obj_t * ui_LblAutoCalPlsDtl = NULL;
lv_obj_t * ui_LblAutoCalPlsQTY = NULL;
lv_obj_t * ui_LblPlsMax = NULL;
lv_obj_t * ui_LblPlsMaxQTY = NULL;



typedef struct {
    lv_obj_t * leaving_item;
} setting_anim_done_data_t;

/* ========================= Internal State ========================= */

static lv_obj_t *setting_items[SETTING_ITEM_COUNT];
static int setting_open_target_index = -1; // -1 یعنی هیچ هدفی برای باز شدن نیست
static bool setting_first_layout = true;
static bool setting_animating = false;
static int prev_visible[3] = { -1, -1, -1 };   // top, center, bottom
static bool setting_force_update = false;
static lv_timer_t * g_transition_timer = NULL;

/* ========================= Helpers ========================= */

static void ui_Setting_cancel_all_anims(void)
{
    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        if(setting_items[i]) {
            lv_anim_delete(setting_items[i], NULL);
            lv_anim_delete(setting_items[i], setting_anim_set_y_cb);
        }
    }

    if (ui_SwAutoCal)     lv_anim_delete(ui_SwAutoCal, NULL);
    if (ui_LblAutoCalOn)  lv_anim_delete(ui_LblAutoCalOn, NULL);
    if (ui_LblAutoCalOff) lv_anim_delete(ui_LblAutoCalOff, NULL);
}

void ui_Setting_force_refresh(void)
{
    setting_force_update = true;
    setting_first_layout = true;

    prev_visible[0] = -1;
    prev_visible[1] = -1;
    prev_visible[2] = -1;
}

static void setting_anim_set_y_cb(void * var, int32_t v)
{
    lv_obj_t * obj = (lv_obj_t *)var;
    if(obj == NULL) return;
    lv_obj_set_y(obj, v);
}

static void setting_hide_all_except(int focus_idx)
{
    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        lv_obj_t * item = setting_items[i];
        if(item == NULL) continue;

        if(i == focus_idx) {
            lv_obj_clear_flag(item, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(item, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ui_Setting_hide_all_details(void)
{
    if (ui_SwAutoCal) {
        lv_obj_add_flag(ui_SwAutoCal, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_LblAutoCalOn) {
        lv_obj_add_flag(ui_LblAutoCalOn, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_LblAutoCalOff) {
        lv_obj_add_flag(ui_LblAutoCalOff, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_LblAutoCalPlsDtl) {
        lv_obj_add_flag(ui_LblAutoCalPlsDtl, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_LblAutoCalPlsQTY) {
        lv_obj_add_flag(ui_LblAutoCalPlsQTY, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_LblPlsMax) {
        lv_obj_add_flag(ui_LblPlsMax, LV_OBJ_FLAG_HIDDEN);
    }
    if (ui_LblPlsMaxQTY) {
        lv_obj_add_flag(ui_LblPlsMaxQTY, LV_OBJ_FLAG_HIDDEN);
    }


}

static void setting_open_anim_ready_cb(lv_anim_t * a)
{
    LV_UNUSED(a);
    setting_open_anim_running = false;
    
    if (setting_open_target_index != -1) {
        ui_Setting_render_detail(setting_open_target_index);
    }
}

void ui_Setting_focus_open(int focus_idx)
{
    if(focus_idx < 0 || focus_idx >= SETTING_ITEM_COUNT) return;
    if(setting_open_anim_running || setting_close_anim_running) return; 
    lv_obj_t * selected = setting_items[focus_idx];
    if(selected == NULL) return;
    setting_open_anim_running = true;
    setting_close_anim_running = false; 
    setting_open_target_index = focus_idx;
    
    setting_hide_all_except(focus_idx);
    lv_obj_move_foreground(selected);

    lv_obj_set_y(selected, setting_selected_closed_y);
    lv_anim_delete(selected, setting_anim_set_y_cb);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, selected);
    lv_anim_set_exec_cb(&a, setting_anim_set_y_cb);
    lv_anim_set_values(&a, setting_selected_closed_y, setting_selected_open_y);
    lv_anim_set_time(&a, SETTING_OK_ANIM_TIME_MS);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&a, setting_open_anim_ready_cb);
    lv_anim_start(&a);
}

bool ui_Setting_focus_open_done(void)
{
    return !setting_open_anim_running;
}

static void setting_close_anim_ready_cb(lv_anim_t * a)
{
    LV_UNUSED(a);
    setting_close_anim_running = false;
}

void ui_Setting_focus_close(int focus_idx)
{
    if(focus_idx < 0 || focus_idx >= SETTING_ITEM_COUNT) return;
    setting_open_target_index = -1;
    lv_obj_t * selected = setting_items[focus_idx];
    if(selected == NULL) return;

    setting_restore_focus_idx = focus_idx;
    setting_close_anim_running = true;
    setting_open_anim_running = false;

    lv_anim_delete(selected, setting_anim_set_y_cb);
    lv_obj_set_y(selected, setting_selected_open_y);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, selected);
    lv_anim_set_exec_cb(&a, setting_anim_set_y_cb);
    lv_anim_set_values(&a, setting_selected_open_y, setting_selected_closed_y);
    lv_anim_set_time(&a, SETTING_OK_ANIM_TIME_MS);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&a, setting_close_anim_ready_cb);
    lv_anim_start(&a);
}

bool ui_Setting_focus_close_done(void)
{
    return !setting_close_anim_running;
}

static int setting_wrap_index(int index)
{
    int res = index % SETTING_ITEM_COUNT;
    if(res < 0) res += SETTING_ITEM_COUNT;
    return res;
}

static bool setting_is_visible_index(int index)
{
    return index == prev_visible[0] ||
           index == prev_visible[1] ||
           index == prev_visible[2];
}

static void setting_prepare_selected_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(obj, SETTING_SELECTED_W, SETTING_SELECTED_H);
    lv_obj_set_y(obj, y);
}

static void setting_prepare_side_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(obj, SETTING_SIDE_W, SETTING_SIDE_H);
    lv_obj_set_y(obj, y);
}

static void setting_prepare_entering_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(obj, 1, 1);
    lv_obj_set_y(obj, y);
}

static void setting_prepare_leaving_item(lv_obj_t *obj, int32_t y)
{
    if(obj == NULL) return;
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(obj, SETTING_SIDE_W, SETTING_SIDE_H);
    lv_obj_set_y(obj, y);
}

static void setting_store_current_triplet(int top, int center, int bottom)
{
    prev_visible[0] = top;
    prev_visible[1] = center;
    prev_visible[2] = bottom;
}

/* ========================= Animations ========================= */

lv_anim_t * moveUp_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!user_data) return NULL;

    user_data->target = TargetObject;
    user_data->val = -1;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_duration(&a, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&a, user_data);
    lv_anim_set_custom_exec_cb(&a, _ui_anim_callback_set_y);
    lv_anim_set_values(&a, 0, -SETTING_SIDE_OFFSET_Y);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_deleted_cb(&a, _ui_anim_callback_free_user_data);
    lv_anim_set_get_value_cb(&a, &_ui_anim_callback_get_y);

    return lv_anim_start(&a);
}

lv_anim_t * moveDown_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!user_data) return NULL;

    user_data->target = TargetObject;
    user_data->val = -1;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, TargetObject);
    lv_anim_set_duration(&a, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&a, user_data);
    lv_anim_set_custom_exec_cb(&a, _ui_anim_callback_set_y);
    lv_anim_set_values(&a, 0, SETTING_SIDE_OFFSET_Y);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_deleted_cb(&a, _ui_anim_callback_free_user_data);
    lv_anim_set_get_value_cb(&a, &_ui_anim_callback_get_y);

    return lv_anim_start(&a);
}

lv_anim_t * fadeout_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * width_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!width_user_data) return NULL;

    width_user_data->target = TargetObject;
    width_user_data->val = -1;

    lv_anim_t width_anim;
    lv_anim_init(&width_anim);
    lv_anim_set_var(&width_anim, TargetObject);
    lv_anim_set_duration(&width_anim, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&width_anim, width_user_data);
    lv_anim_set_custom_exec_cb(&width_anim, _ui_anim_callback_set_width);
    lv_anim_set_values(&width_anim, SETTING_SIDE_W, 0);
    lv_anim_set_path_cb(&width_anim, lv_anim_path_linear);
    lv_anim_set_delay(&width_anim, delay);
    lv_anim_set_deleted_cb(&width_anim, _ui_anim_callback_free_user_data);
    lv_anim_start(&width_anim);

    ui_anim_user_data_t * height_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!height_user_data) return NULL;

    height_user_data->target = TargetObject;
    height_user_data->val = -1;

    lv_anim_t height_anim;
    lv_anim_init(&height_anim);
    lv_anim_set_var(&height_anim, TargetObject);
    lv_anim_set_duration(&height_anim, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&height_anim, height_user_data);
    lv_anim_set_custom_exec_cb(&height_anim, _ui_anim_callback_set_height);
    lv_anim_set_values(&height_anim, SETTING_SIDE_H, 1);
    lv_anim_set_path_cb(&height_anim, lv_anim_path_linear);
    lv_anim_set_delay(&height_anim, delay);
    lv_anim_set_deleted_cb(&height_anim, _ui_anim_callback_free_user_data);

    return lv_anim_start(&height_anim);
}

lv_anim_t * fadein_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * height_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!height_user_data) return NULL;

    height_user_data->target = TargetObject;
    height_user_data->val = -1;

    lv_anim_t height_anim;
    lv_anim_init(&height_anim);
    lv_anim_set_var(&height_anim, TargetObject);
    lv_anim_set_duration(&height_anim, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&height_anim, height_user_data);
    lv_anim_set_custom_exec_cb(&height_anim, _ui_anim_callback_set_height);
    lv_anim_set_values(&height_anim, 0, SETTING_SIDE_H);
    lv_anim_set_path_cb(&height_anim, lv_anim_path_linear);
    lv_anim_set_delay(&height_anim, delay);
    lv_anim_set_deleted_cb(&height_anim, _ui_anim_callback_free_user_data);
    lv_anim_start(&height_anim);

    ui_anim_user_data_t * width_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    if(!width_user_data) return NULL;

    width_user_data->target = TargetObject;
    width_user_data->val = -1;

    lv_anim_t width_anim;
    lv_anim_init(&width_anim);
    lv_anim_set_var(&width_anim, TargetObject);
    lv_anim_set_duration(&width_anim, SETTING_ANIM_TIME_MS);
    lv_anim_set_user_data(&width_anim, width_user_data);
    lv_anim_set_custom_exec_cb(&width_anim, _ui_anim_callback_set_width);
    lv_anim_set_values(&width_anim, 0, SETTING_SIDE_W);
    lv_anim_set_path_cb(&width_anim, lv_anim_path_linear);
    lv_anim_set_delay(&width_anim, delay);
    lv_anim_set_deleted_cb(&width_anim, _ui_anim_callback_free_user_data);

    return lv_anim_start(&width_anim);
}

/* ========================= Transition Logic ========================= */

static void setting_transition_done_timer_cb(lv_timer_t *timer)
{
    setting_anim_done_data_t *data =
        (setting_anim_done_data_t *)lv_timer_get_user_data(timer);

    if(data != NULL) {
        if(data->leaving_item != NULL) {
            lv_obj_add_flag(data->leaving_item, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(data->leaving_item, SETTING_SIDE_W, SETTING_SIDE_H);
        }
        lv_free(data);
    }

    setting_animating = false;
    g_transition_timer = NULL;
    lv_timer_delete(timer);
}

static void setting_transition_down(int leaving_idx, int moving1_idx, int moving2_idx, int entering_idx)
{
    lv_obj_t *leaving  = setting_items[leaving_idx];
    lv_obj_t *moving1  = setting_items[moving1_idx];
    lv_obj_t *moving2  = setting_items[moving2_idx];
    lv_obj_t *entering = setting_items[entering_idx];

    setting_prepare_leaving_item(leaving, -SETTING_SIDE_OFFSET_Y);
    setting_prepare_selected_item(moving1, 0);
    setting_prepare_side_item(moving2, SETTING_SIDE_OFFSET_Y);
    setting_prepare_entering_item(entering, SETTING_SIDE_OFFSET_Y);

    moveUp_Animation(moving1, 0);
    moveUp_Animation(moving2, 0);
    fadeout_Animation(leaving, 0);
    fadein_Animation(entering, 0);

    lv_obj_move_foreground(moving2);

    setting_anim_done_data_t *done = lv_malloc(sizeof(setting_anim_done_data_t));
    if(done != NULL) {
        done->leaving_item = leaving;
        g_transition_timer = lv_timer_create(
            setting_transition_done_timer_cb,
            SETTING_ANIM_TIME_MS + 20,
            done
        );
        lv_timer_set_repeat_count(g_transition_timer, 1);
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

    setting_prepare_entering_item(entering, -SETTING_SIDE_OFFSET_Y);
    setting_prepare_side_item(moving1, -SETTING_SIDE_OFFSET_Y);
    setting_prepare_selected_item(moving2, 0);
    setting_prepare_leaving_item(leaving, SETTING_SIDE_OFFSET_Y);

    moveDown_Animation(moving1, 0);
    moveDown_Animation(moving2, 0);
    fadeout_Animation(leaving, 0);
    fadein_Animation(entering, 0);

    lv_obj_move_foreground(moving1);

    setting_anim_done_data_t *done = lv_malloc(sizeof(setting_anim_done_data_t));
    if(done != NULL) {
        done->leaving_item = leaving;
        g_transition_timer = lv_timer_create(
            setting_transition_done_timer_cb,
            SETTING_ANIM_TIME_MS + 20,
            done
        );
        lv_timer_set_repeat_count(g_transition_timer, 1);
    } else {
        setting_animating = false;
    }
}

static bool setting_transition_to(int new_selected_index)
{
    new_selected_index = setting_wrap_index(new_selected_index);

    const int old_selected = prev_visible[1];

    const bool moving_down =
        old_selected >= 0 &&
        setting_wrap_index(old_selected + 1) == new_selected_index;

    const bool moving_up =
        old_selected >= 0 &&
        setting_wrap_index(old_selected - 1) == new_selected_index;

    const int new_top = setting_wrap_index(new_selected_index - 1);
    const int new_center = new_selected_index;
    const int new_bottom = setting_wrap_index(new_selected_index + 1);

    if (setting_first_layout ||
        prev_visible[0] < 0 ||
        prev_visible[1] < 0 ||
        prev_visible[2] < 0) {

        for (int i = 0; i < SETTING_ITEM_COUNT; i++) {
            if (setting_items[i] != NULL) {
                lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_size(setting_items[i], SETTING_SIDE_W, SETTING_SIDE_H);
            }
        }

        setting_prepare_side_item(setting_items[new_top], -SETTING_SIDE_OFFSET_Y);
        setting_prepare_selected_item(setting_items[new_center], 0);
        setting_prepare_side_item(setting_items[new_bottom], SETTING_SIDE_OFFSET_Y);
        lv_obj_move_foreground(setting_items[new_center]);

        setting_store_current_triplet(new_top, new_center, new_bottom);

        setting_first_layout = false;
        setting_animating = false;
        return true;
    }

    if (setting_animating) {
        return false;
    }

    setting_animating = true;

    if (moving_down) {
        setting_transition_down(prev_visible[0], prev_visible[1], prev_visible[2], new_bottom);
    }
    else if (moving_up) {
        setting_transition_up(new_top, prev_visible[0], prev_visible[1], prev_visible[2]);
    }
    else {
        for (int i = 0; i < SETTING_ITEM_COUNT; i++) {
            if (setting_items[i] != NULL) {
                lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_size(setting_items[i], SETTING_SIDE_W, SETTING_SIDE_H);
            }
        }

        setting_prepare_side_item(setting_items[new_top], -SETTING_SIDE_OFFSET_Y);
        setting_prepare_selected_item(setting_items[new_center], 0);
        setting_prepare_side_item(setting_items[new_bottom], SETTING_SIDE_OFFSET_Y);
        lv_obj_move_foreground(setting_items[new_center]);

        setting_animating = false;
    }

    setting_store_current_triplet(new_top, new_center, new_bottom);
    return true;
}

static void setting_update_visual_state(int setting_index)
{
    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        lv_obj_t *item = setting_items[i];

        if(item == NULL) {
            continue;
        }

        // پیدا کردن آبجکت فرزند (برچسب/Label دکمه) جهت تغییر شفافیت متن آن
        lv_obj_t *label = lv_obj_get_child(item, 0);

        if(i == setting_index) {
            lv_obj_set_style_bg_color(item, SETTING_ITEM_COLOR_SELECTED, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(item, SETTING_SELECTED_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_size(item, SETTING_SELECTED_W, SETTING_SELECTED_H);
            
            if (label != NULL) {
                lv_obj_set_style_text_opa(label, SETTING_TEXT_OPA_SELECTED, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
        else if(setting_is_visible_index(i)) {
            lv_obj_set_style_bg_color(item, SETTING_ITEM_COLOR_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(item, SETTING_SIDE_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_size(item, SETTING_SIDE_W, SETTING_SIDE_H);
            
            if (label != NULL) {
                lv_obj_set_style_text_opa(label, SETTING_TEXT_OPA_SIDE, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
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

/* ========================= Public API ========================= */

bool ui_Setting_update_view(int setting_index)
{
    int wrapped = setting_wrap_index(setting_index);

    if (wrapped == prev_visible[1] && !setting_first_layout && !setting_force_update) {
        return true;
    }

    bool changed = setting_transition_to(wrapped);
    if (!changed) {
        return false;
    }

    setting_force_update = false;
    setting_update_visual_state(wrapped);
    return true;
}

void ui_Setting_render_detail(int setting_index)
{
    const system_settings_t *settings = brain_get_settings();

    ui_Setting_hide_all_details();

    switch (setting_index) {
        case SETTING_ITEM_AUTOCAL:
            if (ui_SwAutoCal) {
                lv_obj_remove_flag(ui_SwAutoCal, LV_OBJ_FLAG_HIDDEN);
                if (settings->auto_cal) {
                    lv_obj_add_state(ui_SwAutoCal, LV_STATE_CHECKED);
                } else {
                    lv_obj_remove_state(ui_SwAutoCal, LV_STATE_CHECKED);
                }
            }

            if (ui_LblAutoCalOn) {
                lv_obj_remove_flag(ui_LblAutoCalOn, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_style_text_opa(
                    ui_LblAutoCalOn,
                    settings->auto_cal ? LV_OPA_COVER : LV_OPA_30,
                    LV_PART_MAIN
                );
            }

            if (ui_LblAutoCalOff) {
                lv_obj_remove_flag(ui_LblAutoCalOff, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_style_text_opa(
                    ui_LblAutoCalOff,
                    settings->auto_cal ? LV_OPA_30 : LV_OPA_COVER,
                    LV_PART_MAIN
                );
            }
            break;

        case SETTING_ITEM_AUTOCAL_PLS:
            if (ui_LblAutoCalPlsDtl) {
                lv_obj_remove_flag(ui_LblAutoCalPlsDtl, LV_OBJ_FLAG_HIDDEN);
            }

            if (ui_LblAutoCalPlsQTY) {
                static char qty_buf[12];
                lv_snprintf(qty_buf, sizeof(qty_buf), "%ld", (long)settings->auto_cal_pls);
                lv_label_set_text(ui_LblAutoCalPlsQTY, qty_buf);
                lv_obj_remove_flag(ui_LblAutoCalPlsQTY, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        case SETTING_ITEM_PULS_MAX:
            if (ui_LblPlsMax) {
                lv_obj_remove_flag(ui_LblPlsMax, LV_OBJ_FLAG_HIDDEN);
            }

            if (ui_LblPlsMaxQTY) {
                static char qty_buf[12];
                lv_snprintf(qty_buf, sizeof(qty_buf), "%ld", (long)settings->puls_max);
                lv_label_set_text(ui_LblPlsMaxQTY, qty_buf);
                lv_obj_remove_flag(ui_LblPlsMaxQTY, LV_OBJ_FLAG_HIDDEN);
            }
            break;


        default:
            break;
    }
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
    lv_obj_set_width(ui_AutoCal, SETTING_SELECTED_W);
    lv_obj_set_height(ui_AutoCal, SETTING_SELECTED_H);
    lv_obj_set_align(ui_AutoCal, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_AutoCal, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblAutoCal = lv_label_create(ui_AutoCal);
    lv_obj_set_align(ui_LblAutoCal, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCal, "کالیبره خودکار");
    lv_obj_set_style_text_font(ui_LblAutoCal, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_AutoCalPls = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_AutoCalPls, SETTING_SELECTED_W);
    lv_obj_set_height(ui_AutoCalPls, SETTING_SELECTED_H);
    lv_obj_set_align(ui_AutoCalPls, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_AutoCalPls, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblAutoCalPls = lv_label_create(ui_AutoCalPls);
    lv_obj_set_align(ui_LblAutoCalPls, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalPls, "تعداد پالس کالیبره");
    lv_obj_set_style_text_font(ui_LblAutoCalPls, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_PulsMax = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_PulsMax, SETTING_SELECTED_W);
    lv_obj_set_height(ui_PulsMax, SETTING_SELECTED_H);
    lv_obj_set_align(ui_PulsMax, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_PulsMax, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblPalsMax = lv_label_create(ui_PulsMax);
    lv_obj_set_align(ui_LblPalsMax, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPalsMax, "حداکثر پالس");
    lv_obj_set_style_text_font(ui_LblPalsMax, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_DelayTime = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_DelayTime, SETTING_SELECTED_W);
    lv_obj_set_height(ui_DelayTime, SETTING_SELECTED_H);
    lv_obj_set_align(ui_DelayTime, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_DelayTime, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblDelayTime = lv_label_create(ui_DelayTime);
    lv_obj_set_align(ui_LblDelayTime, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblDelayTime, "زمان تأخیر");
    lv_obj_set_style_text_font(ui_LblDelayTime, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_StopTrg = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_StopTrg, SETTING_SELECTED_W);
    lv_obj_set_height(ui_StopTrg, SETTING_SELECTED_H);
    lv_obj_set_align(ui_StopTrg, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_StopTrg, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblStopTrg = lv_label_create(ui_StopTrg);
    lv_obj_set_align(ui_LblStopTrg, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblStopTrg, "تریگر توقف");
    lv_obj_set_style_text_font(ui_LblStopTrg, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Beep = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_Beep, SETTING_SELECTED_W);
    lv_obj_set_height(ui_Beep, SETTING_SELECTED_H);
    lv_obj_set_align(ui_Beep, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_Beep, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBeep = lv_label_create(ui_Beep);
    lv_obj_set_align(ui_LblBeep, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBeep, "بوق");
    lv_obj_set_style_text_font(ui_LblBeep, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlAutoOff = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlAutoOff, SETTING_SELECTED_W);
    lv_obj_set_height(ui_BlAutoOff, SETTING_SELECTED_H);
    lv_obj_set_align(ui_BlAutoOff, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlAutoOff, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlAutoOff = lv_label_create(ui_BlAutoOff);
    lv_obj_set_align(ui_LblBlAutoOff, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlAutoOff, "خاموشی خودکار بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlAutoOff, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlAutoConnect = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlAutoConnect, SETTING_SELECTED_W);
    lv_obj_set_height(ui_BlAutoConnect, SETTING_SELECTED_H);
    lv_obj_set_align(ui_BlAutoConnect, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlAutoConnect, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlAutoConnect = lv_label_create(ui_BlAutoConnect);
    lv_obj_set_align(ui_LblBlAutoConnect, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlAutoConnect, "اتصال خودکار بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlAutoConnect, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlPass = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlPass, SETTING_SELECTED_W);
    lv_obj_set_height(ui_BlPass, SETTING_SELECTED_H);
    lv_obj_set_align(ui_BlPass, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlPass, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlPass = lv_label_create(ui_BlPass);
    lv_obj_set_align(ui_LblBlPass, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlPass, "رمز بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlPass, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BlName = lv_button_create(ui_Setting);
    lv_obj_set_width(ui_BlName, SETTING_SELECTED_W);
    lv_obj_set_height(ui_BlName, SETTING_SELECTED_H);
    lv_obj_set_align(ui_BlName, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_BlName, LV_OBJ_FLAG_SCROLLABLE);
    ui_LblBlName = lv_label_create(ui_BlName);
    lv_obj_set_align(ui_LblBlName, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblBlName, "نام بلوتوث");
    lv_obj_set_style_text_font(ui_LblBlName, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SwAutoCal = lv_switch_create(ui_Setting);
    lv_obj_set_width(ui_SwAutoCal, 76);
    lv_obj_set_height(ui_SwAutoCal, 25);
    lv_obj_set_x(ui_SwAutoCal, 0);
    lv_obj_set_y(ui_SwAutoCal, 16);
    lv_obj_set_align(ui_SwAutoCal, LV_ALIGN_CENTER);

    ui_LblAutoCalOn = lv_label_create(ui_Setting);
    lv_obj_set_width(ui_LblAutoCalOn, LV_SIZE_CONTENT);   
    lv_obj_set_height(ui_LblAutoCalOn, LV_SIZE_CONTENT);    
    lv_obj_set_x(ui_LblAutoCalOn, 70);
    lv_obj_set_y(ui_LblAutoCalOn, 16);
    lv_obj_set_align(ui_LblAutoCalOn, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalOn, "روشن");
    lv_obj_set_style_text_font(ui_LblAutoCalOn, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblAutoCalOff = lv_label_create(ui_Setting);
    lv_obj_set_width(ui_LblAutoCalOff, LV_SIZE_CONTENT);   
    lv_obj_set_height(ui_LblAutoCalOff, LV_SIZE_CONTENT);    
    lv_obj_set_x(ui_LblAutoCalOff, -75);
    lv_obj_set_y(ui_LblAutoCalOff, 16);
    lv_obj_set_align(ui_LblAutoCalOff, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalOff, "خاموش");
    lv_obj_set_style_text_font(ui_LblAutoCalOff, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblAutoCalPlsDtl = lv_label_create(ui_Setting);
    lv_obj_set_width(ui_LblAutoCalPlsDtl, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblAutoCalPlsDtl, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblAutoCalPlsDtl, 0);
    lv_obj_set_y(ui_LblAutoCalPlsDtl, 10);
    lv_obj_set_align(ui_LblAutoCalPlsDtl, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalPlsDtl, "تعداد پالس رفرنس");
    lv_obj_set_style_text_font(ui_LblAutoCalPlsDtl, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_LblAutoCalPlsDtl, LV_OBJ_FLAG_HIDDEN);

    ui_LblAutoCalPlsQTY = lv_label_create(ui_Setting);
    lv_obj_set_width(ui_LblAutoCalPlsQTY, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblAutoCalPlsQTY, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblAutoCalPlsQTY, 0);
    lv_obj_set_y(ui_LblAutoCalPlsQTY, 45);
    lv_obj_set_align(ui_LblAutoCalPlsQTY, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblAutoCalPlsQTY, "16");
    lv_obj_set_style_text_font(ui_LblAutoCalPlsQTY, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_LblAutoCalPlsQTY, LV_OBJ_FLAG_HIDDEN);

    ui_LblPlsMax = lv_label_create(ui_Setting);
    lv_obj_set_width(ui_LblPlsMax, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblPlsMax, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblPlsMax, 0);
    lv_obj_set_y(ui_LblPlsMax, 10);
    lv_obj_set_align(ui_LblPlsMax, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPlsMax, "تعداد پالس هر اسکن");
    lv_obj_set_style_text_font(ui_LblPlsMax, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_LblPlsMax, LV_OBJ_FLAG_HIDDEN);

    ui_LblPlsMaxQTY = lv_label_create(ui_Setting);
    lv_obj_set_width(ui_LblPlsMaxQTY, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblPlsMaxQTY, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblPlsMaxQTY, 0);
    lv_obj_set_y(ui_LblPlsMaxQTY, 45);
    lv_obj_set_align(ui_LblPlsMaxQTY, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPlsMaxQTY, "300");
    lv_obj_set_style_text_font(ui_LblPlsMaxQTY, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_LblPlsMaxQTY, LV_OBJ_FLAG_HIDDEN);



    setting_init_item_array();

    for(int i = 0; i < SETTING_ITEM_COUNT; i++) {
        if(setting_items[i]) {
            lv_obj_add_flag(setting_items[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
    ui_Setting_hide_all_details();

    prev_visible[0] = -1;
    prev_visible[1] = -1;
    prev_visible[2] = -1;

    setting_first_layout = true;
    setting_animating = false;
    setting_force_update = false;
    setting_open_anim_running = false;
    setting_close_anim_running = false;

    ui_Setting_update_view(brain_get_setting_index());
}

void ui_Setting_screen_destroy(void)
{
    if (g_transition_timer != NULL) {
        setting_anim_done_data_t *data =
            (setting_anim_done_data_t *)lv_timer_get_user_data(g_transition_timer);

        if (data != NULL) {
            lv_free(data);
        }

        lv_timer_delete(g_transition_timer);
        g_transition_timer = NULL;
    }

    ui_Setting_cancel_all_anims();

    setting_animating = false;
    setting_open_anim_running = false;
    setting_close_anim_running = false;
    setting_open_target_index = -1;
    setting_restore_focus_idx = 0;
    setting_first_layout = true;
    setting_force_update = false;

    prev_visible[0] = -1;
    prev_visible[1] = -1;
    prev_visible[2] = -1;

    if (ui_Setting != NULL) {
        lv_obj_delete(ui_Setting);
        ui_Setting = NULL;
    }

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
    ui_LblAutoCalOff = NULL;
    ui_SwAutoCal = NULL;
    ui_LblAutoCalOn = NULL;
    ui_LblAutoCalPlsDtl = NULL;
    ui_LblAutoCalPlsQTY = NULL;
    ui_LblPlsMax = NULL;
    ui_LblPlsMaxQTY = NULL;



    for (int i = 0; i < SETTING_ITEM_COUNT; i++) {
        setting_items[i] = NULL;
    }
}
