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

/**
 *  @page CAN_servizi_page CAN Servizi
 *
 *  CAN servizi network arises from the need to digitize all those signals 
 *  necessary for the operation of the car.
 *  VCU master node is interested to receive pedals position percentage values, 
 *  APPS and brake plausibility from frontal SCU, and torque request limiter 
 *  from TCU.
 *
 *  A CANbus communication protocol has been developed based on the CANopen 
 *  specifications (CiA 301).
 *  Each slave node can be represented by a finite state machine with the 
 *  following states: Initialisation, Pre-operational, Operational, Stopped.
 *  During power-up each node is in the Initialization phase. At the end of this
 *  phase, it attempts to send a boot-up message. As soon as it has been 
 *  successfully sent, it is placed in the Pre-operational state.
 *  Using an NMT master message, the VCU can passes the various slave nodes
 *  between the different Pre-operational, Operational and Stopped states.
 *
 *  <h2>VCU master on power up sequence</h2>
 *  1. **wait for BOOT-UP message from slave nodes** (from frontal SCU and TCU)
 *      | COB-ID (11bits) | data byte 0 |
 *      | :-------------: | :---------: |
 *      | 0x700 + NODE-ID | 0x00        |
 *      
 *  2. **send NMT 'go Operational'** (broadcast)
 *      | COB-ID (11bits) | data byte 0 | data byte 1 |
 *      | :-------------: | :---------: | :---------: |
 *      | 0x000           | 0x01        | 0x00        |
 *
 *  <h2>Fault Tolerance</h2>
 *  If failure of CAN servizi network occurs, redundancy has been introduced in 
 *  the acquisition of pedals: if PDO with the pedal data are not received within a 
 *  certain timeout then CAN servizi network is considered non-functional and 
 *  pedal sensors are acquired directly via analog signals from the VCU.
 */

/** @addtogroup CAN_module_group
 *  
 *  @{
 *
 *      @defgroup CAN_servizi_group CAN servizi
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