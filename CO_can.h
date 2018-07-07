/** 
 *  @file           CO_can.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          CAN setup header module
 */

#ifndef _CO_CAN_H
#define _CO_CAN_H

#include "common.h"
#include "board_pinout.h"
/**
 *  @page CAN CAN Networks
 *
 *  Two CAN networks have been designed to be inserted into the vehicle: a first
 *  CAN network between the VCU and the inverter (CAN funzionale) and a second 
 *  CAN network between the VCU, TCU and SCUs (CAN servizi).
 *
 *  Each node connected to CAN network has an unique ID internally to that network, 
 *  according to this table:
 *  | CAN NETWORK | NODE       | NODE-ID |
 *  | :---------: | :--------: | :-----: |
 *  | FUNZIONALE  | INVERTER   | 1       |
 *  | ^           | VCU        | 2       |
 *  | SERVIZI     | FrontalSCU | 1       |
 *  | ^           | VCU        | 2       |
 *  | ^           | TCU        | 4       |
 *
 *  - @subpage CAN_funzionale_page "CAN funzionale"
 *  - @subpage CAN_servizi_page "CAN servizi"
 */

/**
 *  @defgroup CAN_module_group CAN module
 *  @{
 */

/**
 *  @brief      This function initializes both CAN funzionale and CAN servizi
 *              networks.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @retval     true    CAN networks initialized successfully
 *  @retval     false   CAN networks initialization failed
 */
bool can_init();

/**
 *  @}
 */

#endif /* _CO_CAN_H_ */