#ifndef _CAN_FUNZIONALE_H_
#define _CAN_FUNZIONALE_H_

#include "common.h"

bool can_funzionale_init();

volatile bool can_funzionale_initialized();

volatile bool can_funzionale_online();

void inverter_torque_request(uint16_t torque);

volatile uint16_t get_torque_actual_value();

#endif /* _CAN_FUNZIONALE_H_ */