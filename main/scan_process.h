#ifndef SCAN_PROCESS_H
#define SCAN_PROCESS_H


#include "brain.h"

#define ADC_MAX_RESOLUTION 4095
#define ADC_MID_RESOLUTION (ADC_MAX_RESOLUTION / 2)
#define SCAN_NEEDLE_MAX_ANGLE 900

void scan_process_init(void);
void scan_process_stop(void);
void scan_process_handle_trigger(scan_mode_t mode);




int scan_process_get_current_adc_value(void);
int scan_process_get_signed_value(void);
int scan_process_get_positive_arc_value(void);
int scan_process_get_negative_arc_value(void);
int scan_process_get_needle_angle(void);
int scan_process_get_pulse_count(void);

#endif
