#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

// ADDR pin tied to GND -> 0x48 (change if wired differently)
#define ADS1115_ADDR            0x48

// I2C pins: SDA = GPIO22, SCL = GPIO23
#define ADS1115_I2C_PORT        I2C_NUM_0
#define ADS1115_I2C_SCL_GPIO    23
#define ADS1115_I2C_SDA_GPIO    22
#define ADS1115_I2C_FREQ_HZ     100000

typedef enum
{
    ADS1115_PGA_6_144V = 0,
    ADS1115_PGA_4_096V,
    ADS1115_PGA_2_048V,   // default
    ADS1115_PGA_1_024V,
    ADS1115_PGA_0_512V,
    ADS1115_PGA_0_256V,
} ads1115_pga_t;

typedef enum
{
    ADS1115_DR_8SPS = 0,
    ADS1115_DR_16SPS,
    ADS1115_DR_32SPS,
    ADS1115_DR_64SPS,
    ADS1115_DR_128SPS,    // default
    ADS1115_DR_250SPS,
    ADS1115_DR_475SPS,
    ADS1115_DR_860SPS,
} ads1115_dr_t;

esp_err_t ads1115_init(void);

// Differential read: AIN0(+) - AIN1(-)
esp_err_t ads1115_read_diff_0_1(int16_t *raw, float *voltage_mv);

// Change gain (also updates the mV scale used by reads)
esp_err_t ads1115_set_pga(ads1115_pga_t pga);

#ifdef __cplusplus
}
#endif

#endif // ADS1115_H