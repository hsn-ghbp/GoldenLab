#ifndef BATTERY_PROCESS_H
#define BATTERY_PROCESS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BATTERY_LEVEL_EMPTY = 0,
    BATTERY_LEVEL_25,
    BATTERY_LEVEL_50,
    BATTERY_LEVEL_75,
    BATTERY_LEVEL_FULL
} battery_level_t;

void battery_process_init(void);
void battery_process_update(void);

uint16_t battery_process_get_adc_value(void);
uint16_t battery_process_get_voltage_mv(void);
uint8_t battery_process_get_percent(void);
battery_level_t battery_process_get_level(void);



#ifdef __cplusplus
}
#endif

#endif // BATTERY_PROCESS_H
