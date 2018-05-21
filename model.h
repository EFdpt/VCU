#ifndef _MODEL_H_
#define _MODEL_H_

void model_init();

volatile uint8_t get_tps1_percentage();
volatile uint8_t get_tps2_percentage();
volatile uint8_t get_brake_percentage();
volatile bool    get_apps_plausibility();
volatile bool    get_brake_plausibility();

#endif /* _MODEL_H_ */