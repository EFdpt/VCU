#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include "CAN_ID.h"
#include "board_pinout.h"

#define CAN_FUNZ_BAUDRATE		1000000 // 1Mbit/s
#define CAN_SERV_BAUDRATE		1000000

#define SERIAL_BAUDRATE         115200

#define INVERTER_TORQUE_MIN		0
#define INVERTER_TORQUE_MAX		32767 // da verificare

#define CAN_FUNZ_SYNC_PERIOD    5000 // 5ms
#define CAN_SERVIZI_TIMEOUT_PERIOD		30000	// 30ms

#endif /* _COMMON_H_ */