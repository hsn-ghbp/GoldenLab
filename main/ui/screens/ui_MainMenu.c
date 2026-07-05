#include "../ui.h"
#include <math.h>
#include <stdio.h>

#define MENU_ITEM_COUNT 5

static lv_obj_t *menu_icons[MENU_ITEM_COUNT];
static int menu_selected = 0;

static void menu_update_positions(void);

lv_obj_t * ui_MainMenu = NULL;
lv_obj_t * ui_ScanIcon = NULL;
lv_obj_t * ui_SendDataIcon = NULL;
lv_obj_t * ui_MemoryIcon = NULL;
lv_obj_t * ui_SettingIcon = NULL;
lv_obj_t * ui_InfoIcon = NULL;

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

    menu_update_positions();
}

static void menu_update_positions(void)
{
    printf("Screen W=%d H=%d\n",
           (int)lv_obj_get_width(ui_MainMenu),
           (int)lv_obj_get_height(ui_MainMenu));

    const int cx = 120;
    const int cy = 120;
    const int radius = 75;

    for(int i = 0; i < MENU_ITEM_COUNT; i++)
    {
        int pos = (i - menu_selected + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;

        float angle_deg = -90.0f + pos * (360.0f / MENU_ITEM_COUNT);
        float angle_rad = angle_deg * 3.1415926f / 180.0f;

        int x = cx + (int)(radius * cosf(angle_rad));
        int y = cy + (int)(radius * sinf(angle_rad));

        int w = lv_obj_get_width(menu_icons[i]);
        int h = lv_obj_get_height(menu_icons[i]);

        printf("i=%d pos=%d calc=(%d,%d) size=(%d,%d)\n",
               i, pos, x, y, w, h);

        lv_obj_set_pos(menu_icons[i],
                       x - (w / 2),
                       y - (h / 2));

        printf("real=(%d,%d)\n",
               (int)lv_obj_get_x(menu_icons[i]),
               (int)lv_obj_get_y(menu_icons[i]));

        if(pos == 0)
        {
            lv_image_set_scale(menu_icons[i], 320);
            lv_obj_set_style_opa(menu_icons[i], LV_OPA_COVER, 0);
        }
        else
        {
            lv_image_set_scale(menu_icons[i], 256);
            lv_obj_set_style_opa(menu_icons[i], LV_OPA_70, 0);
        }
    }
}

void menu_next(void)
{
    menu_selected++;

    if(menu_selected >= MENU_ITEM_COUNT)
        menu_selected = 0;

    menu_update_positions();
}

void menu_prev(void)
{
    menu_selected--;

    if(menu_selected < 0)
        menu_selected = MENU_ITEM_COUNT - 1;

    menu_update_positions();
}

void ui_MainMenu_screen_destroy(void)
{
    if(ui_MainMenu)
        lv_obj_del(ui_MainMenu);

    ui_MainMenu = NULL;
    ui_ScanIcon = NULL;
    ui_SendDataIcon = NULL;
    ui_MemoryIcon = NULL;
    ui_SettingIcon = NULL;
    ui_InfoIcon = NULL;
}