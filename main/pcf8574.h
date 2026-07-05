#ifndef PCF8574_H
#define PCF8574_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

//====================================================
// PCF8574
//====================================================

#define PCF8574_ADDR      0x20

//====================================================
// Key scan settings
//====================================================

#define KEY_SCAN_MS       20
#define KEY_DEBOUNCE_MS   40
#define KEY_HOLD_MS       800

//====================================================
// PCF8574 pins
//====================================================

#define KEY_UP_BIT        (1 << 0)   // P0
#define KEY_DOWN_BIT      (1 << 1)   // P1
#define KEY_OK_BIT        (1 << 2)   // P2
#define KEY_BACK_BIT      (1 << 3)   // P3
#define KEY_TRIG_BIT      (1 << 4)   // P4

//====================================================
// Key events
//====================================================

typedef enum
{
    KEY_NONE = 0,

    KEY_UP,
    KEY_DOWN,

    KEY_OK,
    KEY_OK_HOLD,

    KEY_BACK,
    KEY_BACK_HOLD,

    KEY_TRIG

} key_evt_t;

//====================================================
// PCF8574 low level
//====================================================

esp_err_t pcf8574_init(void);
esp_err_t pcf8574_read(uint8_t *value);

//====================================================
// Key manager
//====================================================

void key_init(void);
void key_scan(void);

key_evt_t key_get(void);

#endif // PCF8574_H