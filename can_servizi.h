/** 
 *  @file           can_servizi.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          CAN servizi module header
 */
#ifndef _CAN_SERVIZI_H_
#define _CAN_SERVIZI_H_

#include "common.h"

/** @addtogroup CAN_module_group
 *  
 *  @{
 *
 *      @defgroup CAN_servizi_group
 *      @{
 */

/**
 *  @brief      This function initializes CAN servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN servizi network initialized successfully
 *  @retval     false   CAN servizi network initialization failed
 */
bool can_servizi_init();

/**
 *  @brief      This function returns CAN servizi initialization status.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN servizi network initialized
 *  @retval     false   CAN servizi network not initialized
 */
volatile bool can_servizi_initialized();

/**
 *  @brief      This function send a CANOpen master NMT message for request
 *              'go to Operational' state to CAN servizi slave nodes (SCUs and TCU).
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void can_servizi_go_operational();

/**
 *  @brief      This function returns if CAN servizi network is online.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN servizi network online
 *  @retval     false   CAN servizi network offline
 */
volatile bool can_servizi_online();

/**
 *  @brief      This function returns if TCU node is active and online on the CAN
 *              servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    TCS is online and active
 *  @retval     false   TCS is offline
 */
volatile bool tcs_online();

/**
 *  @brief      This function returns the value of the first APPS in percentage,
 *              retrieved by frontal SCU node over CAN servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     First APPS percentage value
 */
volatile uint8_t get_servizi_tps1();

/**
 *  @brief      This function returns the value of the second APPS in percentage,
 *              retrieved by frontal SCU node over CAN servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Second APPS percentage value
 */
volatile uint8_t get_servizi_tps2();

/**
 *  @brief      This function returns the value of brake pedal position sensor
 *              in percentage, retrieved by frontal SCU node over CAN servizi
 *              network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Brake pedal position percentage value
 */
volatile uint8_t get_servizi_brake();

/**
 *  @brief      This function returns the value of APPS plausibility retrieved 
 *              by frontal SCU node over CAN servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    APPS plausibility
 *  @retval     false   APPS implausibility
 */
volatile bool    get_servizi_apps_plausibility();

/**
 *  @brief      This function returns the value of brake plausibility retrieved 
 *              by frontal SCU node over CAN servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    Brake plausibility
 *  @retval     false   Brake implausibility
 */
volatile bool    get_servizi_brake_plausibility();

/**
 *  @brief      This function returns the value of torque limiter percentage
 *              retrieved by TCU node over CAN servizi network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Torque limiter coefficient in percentage
 */
volatile uint8_t get_tcs_torque_coefficient();

/**
 *      @}
 *  @}
 */

#endif /* _CAN_SERVIZI_H_ */