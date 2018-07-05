/** 
 *  @file           can_funzionale.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          CAN funzionale module header
 */
#ifndef _CAN_FUNZIONALE_H_
#define _CAN_FUNZIONALE_H_

#include "common.h"

/** @addtogroup CAN_module_group
 *  
 *  @{
 *
 *      @defgroup CAN_funzionale_group
 *      @{
 */

/**
 *  @brief      This function initializes CAN funzionale network with inverter.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN funzionale network initialized successfully
 *  @retval     false   CAN funzionale network initialization failed
 */
bool can_funzionale_init();

/**
 *  @brief      This function returns CAN funzionale initialization status.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN funzionale network initialized
 *  @retval     false   CAN funzionale network not initialized
 */
volatile bool can_funzionale_initialized();

/**
 *  @brief      This function returns if CAN funzionale network is online.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN funzionale network online
 *  @retval     false   CAN funzionale network offline
 */
volatile bool can_funzionale_online();

/**
 *  @brief      This function send torque request to inverter.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  torque      Torque value in percentage multiplied per tcs torque limiter coefficient
 */
void inverter_torque_request(uint16_t torque);

/**
 *  @brief      This function send regen request to inverter.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  regen      Regen request value
 */
void inverter_regen_request(uint16_t regen);

/**
 *  @brief      This function return the torque value requested by inverter to motor
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Torque requested by inverter to motor
 */
volatile uint16_t get_torque_actual_value();

/**
 *      @}
 *  @}
 */

#endif /* _CAN_FUNZIONALE_H_ */