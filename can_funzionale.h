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

/**
 *  @page CAN_funzionale_page CAN Funzionale
 *
 *  CAN funzionale network arises from the need to have an high reliable and
 *  robust communication between VCU and inverter.
 * 
 *  <h2>VCU master on power up sequence</h2>
 *  1. **wait for BOOT-UP message from inverter** <br/>
 *      | COB-ID (11bits) | data byte 0 |
 *      | :-------------: | :---------: |
 *      | 0x700 + NODE-ID | 0x00        |
 *      
 * -----------------------------------------------------------
 * 
 *  2. **check VENDOR-ID** (VCU read object 0x1018) <br/>
 *          *VCU send cliend SDO*
 *      | COB-ID (11bits) | Command byte | Obj. Index (2 byte) | Obj. sub-index (byte) | Data (4bytes) |
 *      | :-------------: | :----------: | :-----------------: | :-------------------: | :-----------: |
 *      | 0x600 + NODE-ID | 0x40 | 0x1810 | 0x01 | 0    |
 *      <br/>
 *          *VCU receive server SDO (from inverter)*
 *      | COB-ID (11bits) | Command byte | Obj. Index (2 byte) | Obj. sub-index (byte) | Data (4bytes) |
 *      | :-------------: | :----------: | :-----------------: | :-------------------: | :-----------: |
 *      | 0x580 + NODE-ID | 0x43 | 0x1810 | 0x01 | 0x19030000 |
 *
 * -----------------------------------------------------------
 * 
 *  3. **send NMT 'go Operational'** (broadcast)
 *      | COB-ID (11bits) | data byte 0 | data byte 1 |
 *      | :-------------: | :---------: | :---------: |
 *      | 0x000           | 0x01        | 0x00        |
 *
 * -----------------------------------------------------------
 * 
 *  4. **send PDO to enable PWM** <br/>
 *      | COB-ID (11bits) | Data (RPDO1)            |
 *      | :-------------: | :---------------------: |
 *      | 0x200 + NODE-ID | 06 00 xx xx xx xx xx xx |
 *      <br/>
 *      | COB-ID (11bits) | Data (RPDO1)            |
 *      | :-------------: | :---------------------: |
 *      | 0x200 + NODE-ID | 07 00 xx xx xx xx xx xx |
 *      <br/>
 *      | COB-ID (11bits) | Data (RPDO1)            |
 *      | :-------------: | :---------------------: |
 *      | 0x200 + NODE-ID | 0F 00 xx xx xx xx xx xx |
 *
 * -----------------------------------------------------------
 * 
 *  5. **periodically send SYNC message** <br/>
 *          *VCU broadcast SYNC*
 *      | COB-ID (11bits) | Data byte 0 |
 *      | :-------------: | :---------: |
 *      | 0x80 + NODE-ID  | 0x00        |
 *
 */

/** @addtogroup CAN_module_group
 *  
 *  @{
 *
 *      @defgroup CAN_funzionale_group CAN funzionale
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