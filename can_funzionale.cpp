#include "can_funzionale.h"

#include <due_can.h>

volatile bool inverter_online           = false;
volatile bool inverter_configured       = false;

volatile uint16_t torque_actual_value   = 0;

void can_funzionale_send_sync() {
    CAN_FRAME sync;

    sync.id = 0x00000080;
    sync.length = 1;
    sync.extended = false;
    sync.data.byte[0] = 0x00;
    CAN_FUNZIONALE.sendFrame(sync);
}

void CAN_FUNZ_GENERAL_CB(CAN_FRAME* frame) {

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

  } else if (frame -> id == 0x580 + INVERTER_NODE_ID) {

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

  } else if (frame -> id == 0x180 + INVERTER_NODE_ID) { // inverter torque actual val (Transmit PDO1)
    torque_actual_value = frame -> data.s3;
  }
}

bool can_funzionale_init() {
    if (!CAN_FUNZIONALE.begin(CAN_FUNZ_BAUDRATE))
        return false;
  
    uint32_t i = 0;

    for (; i < 8; i++) {
        CAN_FUNZIONALE.setRXFilter(i, 0, 0, false);
        CAN_FUNZIONALE.setCallback(i, CAN_FUNZ_GENERAL_CB);
    }

    return true;
}

__attribute__((__inline__))
volatile bool can_funzionale_online() {
    return inverter_online && inverter_configured;
}

__attribute__((__inline__)) 
void inverter_torque_request(uint16_t torque) {
    CAN_FRAME output;

    output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
    output.length = 8;
    output.data.high = 0x0F000000;
    output.data.low = 0x00000000 | map(torque, 0, 100, INVERTER_TORQUE_MIN, INVERTER_TORQUE_MAX);
    output.extended = 0;
    CAN_FUNZIONALE.sendFrame(output);
}

__attribute__((__inline__)) 
volatile uint16_t get_torque_actual_value() {
    return torque_actual_value;
}