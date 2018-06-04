#ifndef _CAN_SERVIZI_H_
#define _CAN_SERVIZI_H_

#include "CO_can.h"

bool can_servizi_init();

volatile bool can_servizi_online();

volatile uint8_t get_servizi_tps1();
volatile uint8_t get_servizi_tps2();
volatile uint8_t get_servizi_brake();
volatile bool    get_servizi_apps_plausibility();
volatile bool    get_servizi_brake_plausibility();

#endif /* _CAN_SERVIZI_H_ */