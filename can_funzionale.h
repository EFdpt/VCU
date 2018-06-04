#ifndef _CAN_FUNZIONALE_H_
#define _CAN_FUNZIONALE_H_

bool can_funzionale_init();

void inverter_torque_request(uint16_t torque);

volatile uint16_t get_torque_actual_value();

#endif /* _CAN_FUNZIONALE_H_ */