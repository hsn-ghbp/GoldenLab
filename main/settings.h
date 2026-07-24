#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t autocal_pls;
    uint16_t puls_max;
    uint16_t delay_time;        // ms
    bool stop_trg;
    bool beep;
    bool bl_auto_off;
    bool bl_auto_connect;
    uint32_t bl_pass;
    char bl_name[32];
} app_settings_t;

extern app_settings_t g_settings;

void settings_set_defaults(void);
void settings_load(void);
void settings_save(void);

#endif // SETTINGS_H
