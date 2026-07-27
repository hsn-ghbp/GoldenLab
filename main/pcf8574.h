#ifndef PCF8574_H
#define PCF8574_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
// Direct GPIO key mapping
//--------------------------------------------------

#define KEY_UP_GPIO      GPIO_NUM_13
#define KEY_DOWN_GPIO    GPIO_NUM_12
#define KEY_OK_GPIO      GPIO_NUM_14
#define KEY_BACK_GPIO    GPIO_NUM_27
#define KEY_TRIG_GPIO    GPIO_NUM_26

//--------------------------------------------------
// Key raw bit mapping
// active low:
// 1 = released
// 0 = pressed
//--------------------------------------------------

#define KEY_UP_BIT       (1 << 0)
#define KEY_DOWN_BIT     (1 << 1)
#define KEY_OK_BIT       (1 << 2)
#define KEY_BACK_BIT     (1 << 3)
#define KEY_TRIG_BIT     (1 << 4)

//--------------------------------------------------
// Timing
//--------------------------------------------------

#define KEY_DEBOUNCE_MS  40
#define KEY_HOLD_MS      800
#define KEY_SCAN_MS      10


//--------------------------------------------------
// Key events
//--------------------------------------------------

typedef enum
{
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_OK,
    KEY_BACK,
    KEY_TRIG,
    KEY_OK_HOLD,
    KEY_BACK_HOLD
} key_evt_t;

//--------------------------------------------------
// API
//--------------------------------------------------

esp_err_t pcf8574_init(void);
esp_err_t pcf8574_read(uint8_t *value);

void key_init(void);
void key_scan(void);
key_evt_t key_get(void);

#ifdef __cplusplus
}
#endif

#endif // PCF8574_H
