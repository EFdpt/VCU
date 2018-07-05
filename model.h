/** 
 *  @file           model.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          Board model header file
 */

#ifndef _MODEL_H_
#define _MODEL_H_

#include <Arduino.h>

/** @defgroup Board_model_group
 *  
 *  @{
 */

/**
 *  @brief      This function initializes hardware board.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void model_init();

/**
 *  @brief      This function returns the value of the first APPS in percentage,
 *              retrieved by CAN servizi network, if online, or by analog signal.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     First APPS percentage value
 */
volatile uint8_t get_tps1_percentage();

/**
 *  @brief      This function returns the value of the second APPS in percentage,
 *              retrieved by CAN servizi network, if online, or by analog signal.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Second APPS percentage value
 */
volatile uint8_t get_tps2_percentage();

/**
 *  @brief      This function returns the value of the brake pedal position sensor
 *              in percentage, retrieved by CAN servizi network, if online, or 
 *              by analog signal.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Brake pedal position sensor percentage value
 */
volatile uint8_t get_brake_percentage();

/**
 *  @brief      This function returns the value of APPS plausibility retrieved 
 *              by CAN servizi network, if online, or by analog signal.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    APPS plausibility
 *  @retval     false   APPS implausibility
 */
volatile bool    get_apps_plausibility();

/**
 *  @brief      This function returns the value of brake plausibility retrieved 
 *              by CAN servizi network, if online, or by analog signal.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    Brake plausibility
 *  @retval     false   Brake implausibility
 */
volatile bool    get_brake_plausibility();

/**
 *  @brief      This function returns the value of the SC.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     SC value
 */
volatile uint16_t get_SC_value();

void model_enable_calibrations();
void model_disable_calibrations();

/**
 *  @}
 */

#endif /* _MODEL_H_ */