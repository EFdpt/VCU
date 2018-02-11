#ifndef _COMMON_H_
#define _COMMON_H_

#include <due_can.h>

#define SERIAL_BAUDRATE		9600 // or 115200
#define CAN_BAUDRATE		CAN_BPS_1000K

// VERBOSE_LEVEL:
// 		- 0: no debug
// 		- 1: trace only RX
// 		- 2: trace RX & TX
#define VERBOSE_LEVEL		2

extern void printFrame(CAN_FRAME *frame);

#endif /* _COMMON_H_ */
