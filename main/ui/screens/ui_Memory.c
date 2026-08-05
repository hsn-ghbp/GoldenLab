#include "../ui.h"
#include "brain.h"
#include <stdio.h>
#include<esp_log.h>

lv_obj_t * ui_Memory = NULL;
lv_obj_t * ui_Image2 = NULL;
lv_obj_t * ui_PnlMemInfo = NULL;
lv_obj_t * ui_LblScanCountTxt = NULL;
lv_obj_t * ui_LblSendCountTxt = NULL;
lv_obj_t * ui_LblPercentTxt = NULL;
lv_obj_t * ui_LblScanCount = NULL;
lv_obj_t * ui_LblSendCount = NULL;
lv_obj_t * ui_LblPercent = NULL;
lv_obj_t * ui_BtnDelAll = NULL;
lv_obj_t * ui_LblDelAll = NULL;
lv_obj_t * ui_BtnDelLast = NULL;
lv_obj_t * ui_LblDelLast = NULL;
lv_obj_t * ui_BtnDelSent = NULL;
lv_obj_t * ui_LblDelSent = NULL;
lv_obj_t * ui_PnlWarnning = NULL;
lv_obj_t * ui_LblQuastionPart1 = NULL;
lv_obj_t * ui_LblQuastionPart2 = NULL;
lv_obj_t * ui_LblQuastionPart3 = NULL;

#define TAG_MEM_UI "UI_MEM"

/* انیمیشن */
#define BTN_ANIM_TIME_MS     150
#define BTN_NORMAL_W         170
#define BTN_NORMAL_H         30
#define BTN_SHRINK_DELTA   27
#define BTN_SHRINK_W       (BTN_NORMAL_W - BTN_SHRINK_DELTA)
#define BTN_SHRINK_H       (BTN_NORMAL_H - BTN_SHRINK_DELTA)

/* فوکوس فعلی باتن‌ها: 0=حذف همه, 1=حذف آخرین, 2=حذف ارسال شده */
static int s_mem_focused_btn = 0;


/* ======================== انیمیشن ======================== */

/* ======================== انیمیشن ======================== */


/* 0=حذف همه، 1=حذف آخرین، 2=حذف ارسال‌شده‌ها */

static bool s_mem_animating = false;

/* دکمه‌ای که پس از پایان shrink باید نمایش داده شود */
static lv_obj_t *s_mem_pending_btn = NULL;

static lv_obj_t *mem_get_btn_by_index(int index)
{
    switch (index) {
        case 0:
            return ui_BtnDelAll;

        case 1:
            return ui_BtnDelLast;

        case 2:
            return ui_BtnDelSent;

        default:
            return NULL;
    }
}

static void mem_btn_shrink_anim_cb(void *var, int32_t value)
{
    lv_obj_t *btn = (lv_obj_t *)var;

    int32_t width = BTN_NORMAL_W -
        ((BTN_NORMAL_W - BTN_SHRINK_W) * value) / 1000;

    int32_t height = BTN_NORMAL_H -
        ((BTN_NORMAL_H - BTN_SHRINK_H) * value) / 1000;

    lv_obj_set_size(btn, width, height);
}

static void mem_btn_grow_anim_cb(void *var, int32_t value)
{
    lv_obj_t *btn = (lv_obj_t *)var;

    int32_t width = BTN_SHRINK_W +
        ((BTN_NORMAL_W - BTN_SHRINK_W) * value) / 1000;

    int32_t height = BTN_SHRINK_H +
        ((BTN_NORMAL_H - BTN_SHRINK_H) * value) / 1000;

    lv_obj_set_size(btn, width, height);
}

static void mem_btn_grow_ready_cb(lv_anim_t *anim)
{
    lv_obj_t *btn = (lv_obj_t *)anim->var;

    if (btn != NULL) {
        lv_obj_set_size(btn, BTN_NORMAL_W, BTN_NORMAL_H);
    }

    s_mem_pending_btn = NULL;
    s_mem_animating = false;
}

static void mem_btn_start_grow(lv_obj_t *btn)
{
    if (btn == NULL) {
        s_mem_pending_btn = NULL;
        s_mem_animating = false;
        return;
    }

    lv_obj_set_size(btn, BTN_SHRINK_W, BTN_SHRINK_H);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t grow_anim;
    lv_anim_init(&grow_anim);
    lv_anim_set_var(&grow_anim, btn);
    lv_anim_set_exec_cb(&grow_anim, mem_btn_grow_anim_cb);
    lv_anim_set_values(&grow_anim, 0, 1000);
    lv_anim_set_duration(&grow_anim, BTN_ANIM_TIME_MS);
    lv_anim_set_path_cb(&grow_anim, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&grow_anim, mem_btn_grow_ready_cb);
    lv_anim_start(&grow_anim);
}

static void mem_btn_shrink_ready_cb(lv_anim_t *anim)
{
    lv_obj_t *old_btn = (lv_obj_t *)anim->var;

    if (old_btn != NULL) {
        lv_obj_add_flag(old_btn, LV_OBJ_FLAG_HIDDEN);

        /*
         * اندازه دکمه مخفی را نرمال می‌کنیم تا ورود بعدی آن
         * همیشه از یک وضعیت مشخص شروع شود.
         */
        lv_obj_set_size(old_btn, BTN_NORMAL_W, BTN_NORMAL_H);
    }

    mem_btn_start_grow(s_mem_pending_btn);
}

static void mem_btn_animate_transition(int old_index, int new_index)
{
    if (s_mem_animating) {
        return;
    }

    lv_obj_t *old_btn = mem_get_btn_by_index(old_index);
    lv_obj_t *new_btn = mem_get_btn_by_index(new_index);

    if (old_btn == NULL || new_btn == NULL || old_btn == new_btn) {
        return;
    }

    s_mem_animating = true;
    s_mem_pending_btn = new_btn;

    /* وضعیت اولیه دکمه قبلی */
    lv_anim_delete(old_btn, NULL);
    lv_obj_clear_flag(old_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(old_btn, BTN_NORMAL_W, BTN_NORMAL_H);

    /* دکمه جدید تا پایان shrink مخفی باقی می‌ماند */
    lv_anim_delete(new_btn, NULL);
    lv_obj_add_flag(new_btn, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(new_btn, BTN_SHRINK_W, BTN_SHRINK_H);

    lv_anim_t shrink_anim;
    lv_anim_init(&shrink_anim);
    lv_anim_set_var(&shrink_anim, old_btn);
    lv_anim_set_exec_cb(&shrink_anim, mem_btn_shrink_anim_cb);
    lv_anim_set_values(&shrink_anim, 0, 1000);
    lv_anim_set_duration(&shrink_anim, BTN_ANIM_TIME_MS);
    lv_anim_set_path_cb(&shrink_anim, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&shrink_anim, mem_btn_shrink_ready_cb);
    lv_anim_start(&shrink_anim);
}


/* ======================== ناوبری باتن ======================== */

void ui_Memory_update_btn_visibility(void)
{
    /* مخفی کردن همه */
    if (ui_BtnDelAll)    lv_obj_add_flag(ui_BtnDelAll, LV_OBJ_FLAG_HIDDEN);
    if (ui_BtnDelLast)   lv_obj_add_flag(ui_BtnDelLast, LV_OBJ_FLAG_HIDDEN);
    if (ui_BtnDelSent)   lv_obj_add_flag(ui_BtnDelSent, LV_OBJ_FLAG_HIDDEN);

    /* نمایش فقط دکمه فوکوس شده */
    switch (s_mem_focused_btn) {
        case 0:
            if (ui_BtnDelAll)    lv_obj_clear_flag(ui_BtnDelAll, LV_OBJ_FLAG_HIDDEN);
            break;
        case 1:
            if (ui_BtnDelLast)   lv_obj_clear_flag(ui_BtnDelLast, LV_OBJ_FLAG_HIDDEN);
            break;
        case 2:
            if (ui_BtnDelSent)   lv_obj_clear_flag(ui_BtnDelSent, LV_OBJ_FLAG_HIDDEN);
            break;
    }
}

void ui_Memory_focus_next_btn(void)
{
    if (!ui_Memory_is_ready() || s_mem_animating) {
        return;
    }

    int old_index = s_mem_focused_btn;
    int new_index = (old_index + 1) % 3;

    s_mem_focused_btn = new_index;
    mem_btn_animate_transition(old_index, new_index);
}

void ui_Memory_focus_prev_btn(void)
{
    if (!ui_Memory_is_ready() || s_mem_animating) {
        return;
    }

    int old_index = s_mem_focused_btn;
    int new_index = (old_index - 1 + 3) % 3;

    s_mem_focused_btn = new_index;
    mem_btn_animate_transition(old_index, new_index);
}

int ui_Memory_get_focused_btn(void)
{
    return s_mem_focused_btn;
}




// void ui_Memory_focus_prev_btn(void)
// {
//     if (s_mem_animating) {
//         return; // جلوگیری از ورودی‌های هم‌زمان
//     }

//     int old = s_mem_focused_btn;
//     s_mem_focused_btn = (s_mem_focused_btn - 1 + 3) % 3;

//     if (!s_mem_first_layout) {
//         mem_btn_animate_transition(old, s_mem_focused_btn);
//     } else {
//         s_mem_first_layout = false;
//     }

//     ui_Memory_update_btn_visibility();
// }






void ui_Memory_reset_focus(void)
{
    s_mem_focused_btn = 0;
    s_mem_animating = false;
    s_mem_pending_btn = NULL;

    if (ui_BtnDelAll != NULL) {
        lv_anim_delete(ui_BtnDelAll, NULL);
        lv_obj_set_size(
            ui_BtnDelAll,
            BTN_NORMAL_W,
            BTN_NORMAL_H
        );
        lv_obj_clear_flag(
            ui_BtnDelAll,
            LV_OBJ_FLAG_HIDDEN
        );
    }

    if (ui_BtnDelLast != NULL) {
        lv_anim_delete(ui_BtnDelLast, NULL);
        lv_obj_set_size(
            ui_BtnDelLast,
            BTN_NORMAL_W,
            BTN_NORMAL_H
        );
        lv_obj_add_flag(
            ui_BtnDelLast,
            LV_OBJ_FLAG_HIDDEN
        );
    }

    if (ui_BtnDelSent != NULL) {
        lv_anim_delete(ui_BtnDelSent, NULL);
        lv_obj_set_size(
            ui_BtnDelSent,
            BTN_NORMAL_W,
            BTN_NORMAL_H
        );
        lv_obj_add_flag(
            ui_BtnDelSent,
            LV_OBJ_FLAG_HIDDEN
        );
    }
}


/* ======================== توابع عمومی ======================== */

void ui_memory_set_warning_visible(bool visible)
{
    if (!ui_Memory_is_ready() || !ui_PnlWarnning) return;

    if (visible) {
        lv_obj_remove_flag(ui_PnlWarnning, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(ui_PnlWarnning);
    } else {
        lv_obj_add_flag(ui_PnlWarnning, LV_OBJ_FLAG_HIDDEN);
    }
}

/* 0=حذف همه، 1=حذف آخرین، 2=حذف ارسال‌شده‌ها */
void ui_memory_show_warning(int btn_index)
{
    if (!ui_Memory_is_ready() || !ui_PnlWarnning) return;

    const char *action_text = NULL;

    switch (btn_index) {
        case 0:
            action_text = "حذف همه اسکن ها";
            break;

        case 1:
            action_text = "حذف آخرین اسکن";
            break;

        case 2:
            action_text = "حذف ارسال شده ها";
            break;

        default:
            return;
    }

    if (ui_LblQuastionPart2) {
        lv_label_set_text(ui_LblQuastionPart2, action_text);
    }

    ui_memory_set_warning_visible(true);
}

void ui_Memory_render(void)
{
    if (!ui_Memory_is_ready()) {
        ESP_LOGW(TAG_MEM_UI, "Memory screen is not ready");
        return;
    }

    const size_t scan_count = brain_memory_get_scan_count();
    const size_t sent_scan_count = brain_memory_get_sent_count();
    const uint8_t free_percent = brain_memory_get_free_percent();

    char text[24];

    if (ui_LblScanCount) {
        snprintf(text, sizeof(text), "%zu", scan_count);
        lv_label_set_text(ui_LblScanCount, text);
    }

    if (ui_LblSendCount) {
        snprintf(text, sizeof(text), "%zu", sent_scan_count);
        lv_label_set_text(ui_LblSendCount, text);
    }

    if (ui_LblPercent) {
        snprintf(text, sizeof(text), "%u %%", (unsigned)free_percent);
        lv_label_set_text(ui_LblPercent, text);
    }

    ui_memory_set_warning_visible(false);

    ESP_LOGI(TAG_MEM_UI,
             "Memory UI rendered: scans=%zu, sent=%zu, free=%u%%",
             scan_count,
             sent_scan_count,
             (unsigned)free_percent);
}


/* ======================== ساخت صفحه ======================== */

void ui_Memory_screen_init(void)
{
    ui_Memory = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Memory, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Memory, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Memory, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Image2 = lv_image_create(ui_Memory);
    lv_image_set_src(ui_Image2, &ui_img_memory_png);
    lv_obj_set_width(ui_Image2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image2, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image2, 0);
    lv_obj_set_y(ui_Image2, -97);
    lv_obj_set_align(ui_Image2, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(ui_Image2, LV_OBJ_FLAG_SCROLLABLE);
    lv_image_set_scale(ui_Image2, 200);

    ui_PnlMemInfo = lv_obj_create(ui_Memory);
    lv_obj_set_width(ui_PnlMemInfo, 200);
    lv_obj_set_height(ui_PnlMemInfo, 90);
    lv_obj_set_x(ui_PnlMemInfo, 0);
    lv_obj_set_y(ui_PnlMemInfo, -26);
    lv_obj_set_align(ui_PnlMemInfo, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_PnlMemInfo, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_PnlMemInfo, lv_color_hex(0x83B5E8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PnlMemInfo, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_PnlMemInfo, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_PnlMemInfo, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblScanCountTxt = lv_label_create(ui_PnlMemInfo);
    lv_obj_set_width(ui_LblScanCountTxt, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblScanCountTxt, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblScanCountTxt, 47);
    lv_obj_set_y(ui_LblScanCountTxt, -26);
    lv_obj_set_align(ui_LblScanCountTxt, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblScanCountTxt, "تعداد اسکن");
    lv_obj_set_style_text_font(ui_LblScanCountTxt, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblSendCountTxt = lv_label_create(ui_PnlMemInfo);
    lv_obj_set_width(ui_LblSendCountTxt, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblSendCountTxt, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblSendCountTxt, 28);
    lv_obj_set_y(ui_LblSendCountTxt, 0);
    lv_obj_set_align(ui_LblSendCountTxt, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblSendCountTxt, "اسکن ارسال شده");
    lv_obj_set_style_text_font(ui_LblSendCountTxt, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblPercentTxt = lv_label_create(ui_PnlMemInfo);
    lv_obj_set_width(ui_LblPercentTxt, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblPercentTxt, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblPercentTxt, 33);
    lv_obj_set_y(ui_LblPercentTxt, 26);
    lv_obj_set_align(ui_LblPercentTxt, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPercentTxt, "حافظه باقیمانده");
    lv_obj_set_style_text_font(ui_LblPercentTxt, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblScanCount = lv_label_create(ui_PnlMemInfo);
    lv_obj_set_width(ui_LblScanCount, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblScanCount, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblScanCount, -70);
    lv_obj_set_y(ui_LblScanCount, -26);
    lv_obj_set_align(ui_LblScanCount, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblScanCount, "0");
    lv_obj_set_style_text_font(ui_LblScanCount, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblSendCount = lv_label_create(ui_PnlMemInfo);
    lv_obj_set_width(ui_LblSendCount, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblSendCount, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblSendCount, -70);
    lv_obj_set_y(ui_LblSendCount, 0);
    lv_obj_set_align(ui_LblSendCount, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblSendCount, "0");
    lv_obj_set_style_text_font(ui_LblSendCount, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblPercent = lv_label_create(ui_PnlMemInfo);
    lv_obj_set_width(ui_LblPercent, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblPercent, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblPercent, -70);
    lv_obj_set_y(ui_LblPercent, 26);
    lv_obj_set_align(ui_LblPercent, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblPercent, "100 %");
    lv_obj_set_style_text_font(ui_LblPercent, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========== دکمه ۱: حذف همه اسکن‌ها ========== */
    ui_BtnDelAll = lv_button_create(ui_Memory);
    lv_obj_set_width(ui_BtnDelAll, BTN_NORMAL_W);
    lv_obj_set_height(ui_BtnDelAll, BTN_NORMAL_H);
    lv_obj_set_x(ui_BtnDelAll, 0);
    lv_obj_set_y(ui_BtnDelAll, 56);
    lv_obj_set_align(ui_BtnDelAll, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BtnDelAll, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BtnDelAll, LV_OBJ_FLAG_SCROLLABLE);

    ui_LblDelAll = lv_label_create(ui_BtnDelAll);
    lv_obj_set_width(ui_LblDelAll, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblDelAll, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblDelAll, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblDelAll, "حذف همه اسکن ها");
    lv_obj_set_style_text_font(ui_LblDelAll, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========== دکمه ۲: حذف آخرین اسکن ========== */
    ui_BtnDelLast = lv_button_create(ui_Memory);
    lv_obj_set_width(ui_BtnDelLast, BTN_NORMAL_W);
    lv_obj_set_height(ui_BtnDelLast, BTN_NORMAL_H);
    lv_obj_set_x(ui_BtnDelLast, 0);
    lv_obj_set_y(ui_BtnDelLast, 56);
    lv_obj_set_align(ui_BtnDelLast, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BtnDelLast, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BtnDelLast, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_BtnDelLast, LV_OBJ_FLAG_HIDDEN);

    ui_LblDelLast = lv_label_create(ui_BtnDelLast);
    lv_obj_set_width(ui_LblDelLast, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblDelLast, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblDelLast, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblDelLast, "حذف آخرین اسکن");
    lv_obj_set_style_text_font(ui_LblDelLast, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========== دکمه ۳: حذف ارسال شده‌ها ========== */
    ui_BtnDelSent = lv_button_create(ui_Memory);
    lv_obj_set_width(ui_BtnDelSent, BTN_NORMAL_W);
    lv_obj_set_height(ui_BtnDelSent, BTN_NORMAL_H);
    lv_obj_set_x(ui_BtnDelSent, 0);
    lv_obj_set_y(ui_BtnDelSent, 56);
    lv_obj_set_align(ui_BtnDelSent, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_BtnDelSent, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(ui_BtnDelSent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_BtnDelSent, LV_OBJ_FLAG_HIDDEN);

    ui_LblDelSent = lv_label_create(ui_BtnDelSent);
    lv_obj_set_width(ui_LblDelSent, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblDelSent, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblDelSent, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblDelSent, "حذف ارسال شده ها");
    lv_obj_set_style_text_font(ui_LblDelSent, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ========== پنل هشدار ========== */
    ui_PnlWarnning = lv_obj_create(ui_Memory);
    lv_obj_set_width(ui_PnlWarnning, 180);
    lv_obj_set_height(ui_PnlWarnning, 100);
    lv_obj_set_align(ui_PnlWarnning, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_PnlWarnning, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_PnlWarnning, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_PnlWarnning, lv_color_hex(0xF6E8A2), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PnlWarnning, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_PnlWarnning, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_PnlWarnning, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblQuastionPart1 = lv_label_create(ui_PnlWarnning);
    lv_obj_set_width(ui_LblQuastionPart1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblQuastionPart1, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblQuastionPart1, 0);
    lv_obj_set_y(ui_LblQuastionPart1, -30);
    lv_obj_set_align(ui_LblQuastionPart1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblQuastionPart1, "آیا از");
    lv_obj_set_style_text_color(ui_LblQuastionPart1, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblQuastionPart1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblQuastionPart1, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblQuastionPart2 = lv_label_create(ui_PnlWarnning);
    lv_obj_set_width(ui_LblQuastionPart2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblQuastionPart2, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_LblQuastionPart2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblQuastionPart2, "حذف همه اسکن ها");
    lv_obj_set_style_text_color(ui_LblQuastionPart2, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblQuastionPart2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblQuastionPart2, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblQuastionPart3 = lv_label_create(ui_PnlWarnning);
    lv_obj_set_width(ui_LblQuastionPart3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblQuastionPart3, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_LblQuastionPart3, 0);
    lv_obj_set_y(ui_LblQuastionPart3, 23);
    lv_obj_set_align(ui_LblQuastionPart3, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LblQuastionPart3, "مطمئن هستید؟");
    lv_obj_set_style_text_color(ui_LblQuastionPart3, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblQuastionPart3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblQuastionPart3, &ui_font_vazir20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Memory_reset_focus();
    ui_Memory_update_btn_visibility();
    ui_Memory_render();
}

bool ui_Memory_is_ready(void)
{
    return (ui_Memory != NULL);
}

void ui_Memory_screen_destroy(void)
{
    s_mem_animating = false;
    s_mem_pending_btn = NULL;
    s_mem_focused_btn = 0;

    if (ui_BtnDelAll != NULL) {
        lv_anim_delete(ui_BtnDelAll, NULL);
    }

    if (ui_BtnDelLast != NULL) {
        lv_anim_delete(ui_BtnDelLast, NULL);
    }

    if (ui_BtnDelSent != NULL) {
        lv_anim_delete(ui_BtnDelSent, NULL);
    }

    if (ui_Memory != NULL) {
        lv_obj_delete(ui_Memory);
    }

    ui_Memory = NULL;
    ui_Image2 = NULL;
    ui_PnlMemInfo = NULL;
    ui_LblScanCountTxt = NULL;
    ui_LblSendCountTxt = NULL;
    ui_LblPercentTxt = NULL;
    ui_LblScanCount = NULL;
    ui_LblSendCount = NULL;
    ui_LblPercent = NULL;
    ui_BtnDelAll = NULL;
    ui_LblDelAll = NULL;
    ui_BtnDelLast = NULL;
    ui_LblDelLast = NULL;
    ui_BtnDelSent = NULL;
    ui_LblDelSent = NULL;
    ui_PnlWarnning = NULL;
    ui_LblQuastionPart1 = NULL;
    ui_LblQuastionPart2 = NULL;
    ui_LblQuastionPart3 = NULL;
}
