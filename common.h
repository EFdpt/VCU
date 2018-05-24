#ifndef _COMMON_H_
#define _COMMON_H_

#include "CAN_ID.h"

#define CAN_FUNZIONALE			Can0
#define CAN_SERVIZI				Can1

#define CAN_FUNZ_BAUDRATE		1000000 // 1Mbit/s
#define CAN_SERV_BAUDRATE		1000000

#define SERIAL_BAUDRATE         115200

#define INVERTER_TORQUE_MIN		0
#define INVERTER_TORQUE_MAX		32000 // da verificare

#endif /* _COMMON_H_ */