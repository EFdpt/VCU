/** 
 *  @file           common.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          common macro definitions module
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include "CAN_ID.h"
#include "board_pinout.h"

/**
 *  @defgroup Common_defines_group Common Defines
 *   @{
 */

/**
 *  @def CAN_FUNZ_BAUDRATE
 *  @brief Defines CAN funzionale baudrate
 */
#define CAN_FUNZ_BAUDRATE		1000000 // 1Mbit/s

/**
 *  @def CAN_SERV_BAUDRATE
 *  @brief Defines CAN servizi baudrate
 */
#define CAN_SERV_BAUDRATE		1000000

/**
 *  @def SERIAL_BAUDRATE
 *  @brief Defines serial baudrate
 */
#define SERIAL_BAUDRATE         115200

/**
 *  @def INVERTER_TORQUE_MIN
 *  @brief Defines inverter torque request lower bound
 */
#define INVERTER_TORQUE_MIN		0

/**
 *  @def INVERTER_TORQUE_MAX
 *  @brief Defines inverter torque request upper bound
 */
#define INVERTER_TORQUE_MAX		32767

/**
 *  @def CAN_FUNZ_SYNC_PERIOD
 *  @brief Defines CAN funzionale sync message trasmission period
 */
#define CAN_FUNZ_SYNC_PERIOD    5000 // 5ms

/**
 *  @def CAN_SERVIZI_TIMEOUT_PERIOD
 *  @brief Defines CAN servizi timeout period for fault check
 */
#define CAN_SERVIZI_TIMEOUT_PERIOD		30000	// 30ms

/**
 *  @}
 */

#endif /* _COMMON_H_ */