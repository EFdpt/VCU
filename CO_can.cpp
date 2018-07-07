/** 
 *  @file           CO_can.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          CAN setup module implementation
 */
#include "CO_can.h"
#include "can_servizi.h"
#include "can_funzionale.h"

/** @addtogroup CAN_module_group
 *  
 *  @{
 */


/**
 *  @brief      This function initializes both CAN funzionale and CAN servizi
 *              networks.
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *  @retval     true    CAN networks initialized successfully
 *  @retval     false   CAN networks initialization failed
 */
bool can_init() {
    if (!can_funzionale_initialized())
    	can_funzionale_init();
    if (!can_servizi_initialized())
        return can_servizi_init();
    return true;
}

/** 
 *  @}
 */