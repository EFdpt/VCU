#ifndef _MODEL_H_
#define _MODEL_H_

#include <Arduino.h>

void model_init();

volatile uint8_t get_tps1_percentage();
volatile uint8_t get_tps2_percentage();
volatile uint8_t get_brake_percentage();
volatile bool    get_apps_plausibility();
volatile bool    get_brake_plausibility();

volatile uint16_t get_SC_value();

void model_enable_calibrations();
void model_disable_calibrations();

#endif /* _MODEL_H_ */