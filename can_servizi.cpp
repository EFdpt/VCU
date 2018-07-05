/** 
 *  @file           can_servizi.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          CAN servizi module implementation
 */
#include "can_servizi.h"

#include <due_can.h>
#include <DueTimer.h>

/** @addtogroup CAN_servizi_group
 *  
 *  @{
 */

/**
 * @var     volatile bool can_serv_initialized;
 * @brief   CAN servizi initialization status flag (true if initialized)
 */
volatile bool can_serv_initialized  = false;

/**
 * @var     volatile bool SCU_F_online;
 * @brief   Frontal SCU online status flag (true if online)
 */
volatile bool SCU_F_online          = false;

/**
 * @var     volatile bool TCS_online;
 * @brief   TCS online status flag (true if online)
 */
volatile bool TCS_online            = false;

/**
 * @var     volatile uint32_t curr_pedals_seq_num;
 * @brief   Frontal SCU PDOtx1 current sequence number
 */
volatile uint32_t curr_pedals_seq_num       = 0;

/**
 * @var     volatile uint32_t next_pedals_seq_num;
 * @brief   Frontal SCU PDOtx1 next sequence number
 */
volatile uint32_t next_pedals_seq_num       = 0;

/**
 * @var     volatile uint8_t tps1_percentage;
 * @brief   First APPS percentage value retrieved by frontal SCU node
 */
volatile uint8_t    tps1_percentage    = 0;

/**
 * @var     volatile uint8_t tps2_percentage;
 * @brief   Second APPS percentage value retrieved by frontal SCU node
 */
volatile uint8_t    tps2_percentage    = 0;

/**
 * @var     volatile uint8_t brake_percentage;
 * @brief   Brake pedal position sensor percentage value retrieved by frontal SCU node
 */
volatile uint8_t    brake_percentage   = 0;

/**
 * @var     volatile bool apps_plausibility;
 * @brief   APPS plausibility status retrieved by frontal SCU node
 */
volatile bool       apps_plausibility  = true;

/**
 * @var     volatile bool brake_plausibility;
 * @brief   Brake plausibility status retrieved by frontal SCU node
 */
volatile bool       brake_plausibility = true;

/**
 * @var     volatile uint8_t tcs_coefficient;
 * @brief   torque limiter percentage retrieved by TCU node
 */
volatile uint8_t    tcs_coefficient = 0;

/**
 *  @brief      This function is executed periodically after CAN servizi 
 *              'go Operational' NMT request is sent.
 *              When timeout occurs if @ref next_pedals_seq_num is greater than @ref curr_pedals_seq_num
 *              then frontal SCU is considered active, viceversa it is considered offline.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void timeout() {
    if (next_pedals_seq_num > curr_pedals_seq_num) {
        SCU_F_online = true;
        curr_pedals_seq_num = next_pedals_seq_num;
    } else
        SCU_F_online = false;
}

__attribute__((__inline__))
volatile bool can_servizi_initialized() {
    return can_serv_initialized;
}

/**
 *  @brief      This function manage boot-up messages sent over CAN servizi network
 *              by slave nodes.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  frame  CAN frame received from CAN servizi port
 */
void CAN_SERV_BOOTUP_CB(CAN_FRAME* frame) {
    if (frame -> id == 0x700 + SCU_FRONTAL_NODE_ID)
        SCU_F_online = true;
    else if (frame -> id == 0x700 + TCU_NODE_ID)
        TCS_online = true;
}

/**
 *  @brief      This function manage PDOs received over CAN servizi network and
 *              deserializes data:
 *              <TABLE>
 *                  <TR>
 *                      <TD><b>TPDO num</b></TD><TD><b>NODE-ID</b></TD><TD><b>Length</b></TD><TD><b>Data</b></TD>
 *                  </TR>
 *                  <TR>
 *                      <TD rowspan="5">1</TD><TD rowspan="5">#SCU_FRONTAL_NODE_ID</TD><TD rowspan="5">4</TD>
 *                      <TD>APPS1 percentage</TD>
 *                      <TR><TD>APPS2 percentage</TD></TR>
 *                      <TR><TD>Brake percentage</TD></TR>
 *                      <TR><TD>APPS plausibility</TD></TR>
 *                      <TR><TD>BRAKE plausibility</TD></TR>
 *                  </TR>
 *                  <TR><TD>1</TD><TD>#TCU_NODE_ID</TD><TD>1</TD><TD>TCU torque limiter</TD></TR>
 *              </TABLE>
 *
 *              When PDOtx1 message is received from frontal SCU node then 
 *              @ref next_pedals_seq_num is incremented for keep track of last 
 *              pedals message received.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  frame  CAN frame received from CAN servizi port
 */
void CAN_SERV_GENERAL_CB(CAN_FRAME* frame) {
    switch (frame -> id) {
        case (0x180 + SCU_FRONTAL_NODE_ID): // (pedals)
            tps1_percentage = (uint8_t) frame -> data.byte[0];
            tps2_percentage = (uint8_t) frame -> data.byte[1];
            brake_percentage = (uint8_t) frame -> data.byte[2];
            apps_plausibility = (frame -> data.byte[3] & 0xF0) ? true : false;
            brake_plausibility = (frame -> data.byte[3] & 0x0F) ? true : false;

            // fault check
            next_pedals_seq_num++; // received new frame
            break;
        case (0x180 + TCU_NODE_ID): // TCU coefficient limiter
            tcs_coefficient = (uint8_t) frame -> data.byte[0];
            break;
        default: {}
    }
}

/**
 *  @brief      This function initialize CAN servizi hardware port with baudrate
 *              #CAN_SERV_BAUDRATE.
 *              Mailbox 0 is configured for receiving boot-up messages from CAN
 *              servizi slave nodes (filter = 0x00000700, mask = 0x1FFFFF80);
 *              remaining mailboxes are configured for receiving TPDOs from CAN 
 *              servizi slave nodes (filter = 0x00000080, mask = 0x1FFFFC80).
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @retval     true    CAN servizi initialized
 *  @retval     false   CAN servizi not initialized
 */
bool can_servizi_init() {
    if (!CAN_SERVIZI.begin(CAN_SERV_BAUDRATE))
        return (can_serv_initialized = false);
  
    uint32_t i;

    // set mailbox 0 for receiving boot-up frames
    // FILTER   = 00000000000000000011100000000,
    // MASK     = 11111111111111111111110000000
    CAN_SERVIZI.setRXFilter(0, 0x00000700, 0x1FFFFF80, false);
    CAN_SERVIZI.setCallback(0, CAN_SERV_BOOTUP_CB);

    // set remaining mailboxes for receiving TPDOs
    // FILTER   = 00000000000000000000010000000,
    // MASK     = 11111111111111111110010000000
    for (i = 2; i < 8; i++) {
        CAN_SERVIZI.setRXFilter(i, 0x00000080, 0x1FFFFC80, false);
        CAN_SERVIZI.setCallback(i, CAN_SERV_GENERAL_CB);
    }

    return (can_serv_initialized = true);
}

__attribute__((__inline__))
void can_servizi_go_operational() {
    // broadcast NMT request go Operational
    CAN_FRAME output;
    output.id = 0x00000000 ;
    output.length = 2 ;
    output.data.byte[0] = 0x01;
    output.data.byte[1] = 0x00;
    output.extended = 0;
    CAN_SERVIZI.sendFrame(output);

    DueTimer::getAvailable().attachInterrupt(timeout).start(CAN_SERVIZI_TIMEOUT_PERIOD);
}

__attribute__((__inline__))
volatile bool can_servizi_online() {
    return (can_serv_initialized && SCU_F_online);
}

__attribute__((__inline__))
volatile bool tcs_online() {
    return (can_serv_initialized && TCS_online);
}

__attribute__((__inline__))
volatile uint8_t get_servizi_tps1() {
    return tps1_percentage;
}

__attribute__((__inline__))
volatile uint8_t get_servizi_tps2() {
    return tps2_percentage;
}

__attribute__((__inline__))
volatile uint8_t get_servizi_brake() {
    return brake_percentage;
}

__attribute__((__inline__))
volatile bool    get_servizi_apps_plausibility() {
    return apps_plausibility;
}

__attribute__((__inline__))
volatile bool    get_servizi_brake_plausibility() {
    return brake_plausibility;
}

__attribute__((__inline__))
volatile uint8_t get_tcs_torque_coefficient() {
    return TCS_online ? tcs_coefficient : 100;
}

/**
 *  @}
 */