/** 
 *  @file           can_funzionale.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          CAN funzionale module implementation
 */
#include "can_funzionale.h"
#include "def.h"

#include <due_can.h>
#include <DueTimer.h>

/** @addtogroup CAN_funzionale_group
 *  
 *  @{
 */

/**
 * @var     volatile bool can_funz_initialized;
 * @brief   CAN funzionale initialization status flag (true if initialized)
 */
volatile bool can_funz_initialized          = false;

/**
 * @var     volatile bool inverter_online;
 * @brief   Inverter online status flag (true if online)
 */
volatile bool inverter_online               = false;

/**
 * @var     volatile bool inverter_configured;
 * @brief   Inverter configured status flag (true if configured)
 */
volatile bool inverter_configured           = false;

/**
 * @var     volatile uint16_t torque_actual_value;
 * @brief   Torque requested by inverter to motor
 */
volatile uint16_t torque_actual_value       = 0;

__attribute__((__inline__))
volatile bool can_funzionale_initialized() {
    return can_funz_initialized;
}

/**
 *  @brief      This function sends a periodic CANOpen sync message to inverter
 *              slave node.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void can_funzionale_send_sync() {
    CAN_FRAME sync;

    sync.id = 0x00000080;
    sync.length = 1;
    sync.extended = false;
    sync.data.byte[0] = 0x00;
    CAN_FUNZIONALE.sendFrame(sync);
}

/**
 *  @brief      This function manage boot-up message sent over CAN funzionale network
 *              by inverter slave node.
 *              Upon boot-up message reception the VCU send a SDO client request
 *              for check inverter vendor ID; then inverter is considered online
 *              over CAN funzionale network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  frame  CAN frame received from CAN funzionale port
 */
void CAN_FUNZ_BOOTUP_CB(CAN_FRAME* frame) {
    if (frame -> id == (0x700 + INVERTER_NODE_ID)) {

        if (inverter_online)
          return;

        //Check inverter vendor ID (SDO client request)
        CAN_FRAME output;

        output.id = 0x00000600 + (uint8_t) VCU_NODE_ID ;
        output.length = 8;
        output.data.high = 0x40181001;
        output.data.low = 0x00000000;
        output.extended = 0;
        CAN_FUNZIONALE.sendFrame(output);
        
        inverter_online = true;
    }
}

/**
 *  @brief      This function manage SDO server response with inverter Vendor ID.
 *              VCU sends NMT operational and PDOs to enable PWM; then inverter
 *              is considered correctly configured and a timer is started for
 *              sending periodic sync messages.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  frame  CAN frame received from CAN servizi port
 */
void CAN_FUNZ_VENDOR_ID_CB(CAN_FRAME* frame) {
    if (frame -> id == 0x580 + INVERTER_NODE_ID) {

        if (inverter_configured)
            return;

        CAN_FRAME output;

        //pacchetto NMT comando 0x01=start remote node
        output.id = 0x00000000 ;
        output.length = 2 ;
        output.data.byte[0] = 0x01;
        output.data.byte[1] = 0x00;
        output.extended = 0;
        CAN_FUNZIONALE.sendFrame(output);
        
        //pacchetto "shutdown" transizione 2 pagina 12 Firmware manual
        output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
        output.length = 8;
        output.data.high = 0x06000000;
        output.data.low = 0x00000000;
        output.extended = 0;
        CAN_FUNZIONALE.sendFrame(output);

        //pacchetto "switch on" transizione 3 pagina 12 Firmware manual
        output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
        output.length = 8;
        output.data.high = 0x07000000;
        output.data.low = 0x00000000;
        output.extended = 0;
        CAN_FUNZIONALE.sendFrame(output);

        output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
        output.length = 8;
        output.data.high = 0x0F000000;
        output.data.low = 0x00000000;
        output.extended = 0;
        CAN_FUNZIONALE.sendFrame(output);

        inverter_configured = true;

        // send periodic SYNC
        DueTimer::getAvailable().attachInterrupt(can_funzionale_send_sync).start(CAN_FUNZ_SYNC_PERIOD);
    }
}

/**
 *  @brief      This function manage TPDO from inverter and deserializes data:
 *              <TABLE>
 *                  <TR>
 *                      <TD><b>TPDO num</b></TD><TD><b>NODE-ID</b></TD><TD><b>Data</b></TD>
 *                  </TR>
 *                  <TR>
 *                      <TD>1</TD><TD>#INVERTER_NODE_ID</TD><TD>Torque Actual Val</TD>
 *                  </TR>
 *              </TABLE>
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  frame  CAN frame received from CAN servizi port
 */
void CAN_FUNZ_GENERAL_CB(CAN_FRAME* frame) {
    if (frame -> id == 0x180 + INVERTER_NODE_ID) { // inverter torque actual val (Transmit PDO1)
        torque_actual_value = frame -> data.s3;
    }
}

/**
 *  @brief      This function initialize CAN funzionale hardware port with baudrate
 *              #CAN_FUNZ_BAUDRATE.
 *              Mailbox 0 is configured for receiving boot-up messages from inverter
 *              slave node (filter = 0x00000700 + #INVERTER_NODE_ID, mask = 0x1FFFFFFF);
 *              mailbox 1 is configured for receiving vendorID SDO response from inverter
 *              (filter = 0x00000580 + #INVERTER_NODE_ID, mask = 0x1FFFFFFF);
 *              remaining mailboxes are configured for receiving TPDOs from inverter
 *              slave node (filter = 0x00000080, mask = 0x1FFFFCFF).
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @retval     true    CAN servizi initialized
 *  @retval     false   CAN servizi not initialized
 */
bool can_funzionale_init() {
    if (!CAN_FUNZIONALE.begin(CAN_FUNZ_BAUDRATE))
        return (can_funz_initialized = false);
  
    uint32_t i;

    // set mailbox 0 for receiving boot-up frames
    CAN_FUNZIONALE.setRXFilter(0, 0x00000700 + INVERTER_NODE_ID, 0x1FFFFFFF, false);
    CAN_FUNZIONALE.setCallback(0, CAN_FUNZ_BOOTUP_CB);

    // set mailbox 1 for receiving vendorID frame
    CAN_FUNZIONALE.setRXFilter(1, 0x00000580 + INVERTER_NODE_ID, 0x1FFFFFFF, false);
    CAN_FUNZIONALE.setCallback(1, CAN_FUNZ_VENDOR_ID_CB);

    // set remaining mailboxes for receiving TPDOs (TPDO1, TPDO2, TPDO3)
    for (i = 2; i < 8; i++) {
        CAN_FUNZIONALE.setRXFilter(i, 0x00000080 + INVERTER_NODE_ID, 0x1FFFFCFF, false);
        CAN_FUNZIONALE.setCallback(i, CAN_FUNZ_GENERAL_CB);
    }

    return (can_funz_initialized = true);
}

/**
 *  @brief      This function returns if inverter is online and active over CAN
 *              funzionale.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @retval     true    CAN funzionale initialized, inverter online and inverter configured successfully
 *  @retval     false   CAN funzionale not initialized or inverter not online or configured.
 */
__attribute__((__inline__))
volatile bool can_funzionale_online() {
    return can_funzionale_initialized && inverter_online && inverter_configured;
}

/**
 *  @brief      This function send torque request to inverter.
 *              If inverter is active over CAN funzionale network then the request
 *              is done via RPDO1 viceversa it's done via analog signal.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @param[in]  torque  Torque value in percentage multiplied per tcs torque limiter coefficient
 */
__attribute__((__inline__)) 
void inverter_torque_request(uint16_t torque) {

    if (can_funzionale_online()) {
        CAN_FRAME output;

        output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
        output.length = 8;
        output.data.high = 0x0F000000;
        output.data.low = 0x00000000 | map(torque, 0, 10000, INVERTER_TORQUE_MIN, INVERTER_TORQUE_MAX);
        output.extended = 0;
        CAN_FUNZIONALE.sendFrame(output);
    } else {
        analogWrite(INVERTER_ANALOG_PIN, map(torque, 0, 10000, 0, 4095));
    }
}

__attribute__((__inline__))
void inverter_regen_request(uint16_t regen) {
    if (can_funzionale_online()) {

    } else {
        analogWrite(BRAKE_REGEN_PIN, regen);
    }
}

/**
 *  @brief      This function return the torque value requested by inverter to motor
 *              retrieved from TPDO1 from inverter over CAN funzionale network.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @return     Torque requested by inverter to motor
 */
__attribute__((__inline__)) 
volatile uint16_t get_torque_actual_value() {
    return torque_actual_value;
}

/**
 *  @}
 */