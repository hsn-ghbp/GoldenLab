#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bluetooth_init(const char *device_name);
esp_err_t bluetooth_enable(void);
esp_err_t bluetooth_disable(void);

bool bluetooth_is_enabled(void);
bool bluetooth_is_connected(void);

esp_err_t bluetooth_send_int32(int32_t value);
esp_err_t bluetooth_send_array(const int32_t *data, size_t count);

#ifdef __cplusplus
}
#endif

#endif


