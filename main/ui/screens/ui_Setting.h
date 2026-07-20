#ifndef UI_SETTING_H
#define UI_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ui.h"
#include <stdbool.h>

/* Main screen object */
extern lv_obj_t * ui_Setting;

/* Setting items */
extern lv_obj_t * ui_AutoCal;
extern lv_obj_t * ui_LblAutoCal;

extern lv_obj_t * ui_AutoCalPls;
extern lv_obj_t * ui_LblAutoCalPls;

extern lv_obj_t * ui_PulsMax;
extern lv_obj_t * ui_LblPalsMax;

extern lv_obj_t * ui_DelayTime;
extern lv_obj_t * ui_LblDelayTime;

extern lv_obj_t * ui_StopTrg;
extern lv_obj_t * ui_LblStopTrg;

extern lv_obj_t * ui_Beep;
extern lv_obj_t * ui_LblBeep;

extern lv_obj_t * ui_BlAutoOff;
extern lv_obj_t * ui_LblBlAutoOff;

extern lv_obj_t * ui_BlAutoConnect;
extern lv_obj_t * ui_LblBlAutoConnect;

extern lv_obj_t * ui_BlPass;
extern lv_obj_t * ui_LblBlPass;

extern lv_obj_t * ui_BlName;
extern lv_obj_t * ui_LblBlName;

/* API */
void ui_Setting_screen_init(void);
bool ui_Setting_is_ready(void);
void ui_Setting_update_view(int focus_idx);
void ui_Setting_screen_destroy(void);
void ui_Setting_focus_open(int focus_idx);
void ui_Setting_focus_close(int focus_idx);








#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
