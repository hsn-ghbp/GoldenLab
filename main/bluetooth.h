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
esp_err_t bluetooth_send_raw(const uint8_t *data, size_t len);
esp_err_t bluetooth_send_int32_stream_begin(uint32_t scan_id, const int32_t *data, size_t count);
bool bluetooth_stream_is_active(void);
void bluetooth_stream_abort(void);
bool bluetooth_has_last_peer(void);
//void bluetooth_get_last_peer(uint8_t out_bda[ESP_BD_ADDR_LEN]);
esp_err_t bluetooth_connect_last_peer(void);




#ifdef __cplusplus
}
#endif

#endif


