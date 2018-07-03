#include "can_funzionale.h"
#include "def.h"

#include <due_can.h>
#include <DueTimer.h>

volatile bool can_funz_initialized          = false;
volatile bool inverter_online               = false;
volatile bool inverter_configured           = false;

volatile uint16_t torque_actual_value       = 0;

__attribute__((__inline__))
volatile bool can_funzionale_initialized() {
    return can_funz_initialized;
}

void can_funzionale_send_sync() {
    CAN_FRAME sync;

    sync.id = 0x00000080;
    sync.length = 1;
    sync.extended = false;
    sync.data.byte[0] = 0x00;
    CAN_FUNZIONALE.sendFrame(sync);
}

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

void CAN_FUNZ_GENERAL_CB(CAN_FRAME* frame) {
    if (frame -> id == 0x180 + INVERTER_NODE_ID) { // inverter torque actual val (Transmit PDO1)
        torque_actual_value = frame -> data.s3;
    }
}

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

__attribute__((__inline__))
volatile bool can_funzionale_online() {
    return can_funzionale_initialized && inverter_online && inverter_configured;
}

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

__attribute__((__inline__)) 
volatile uint16_t get_torque_actual_value() {
    return torque_actual_value;
}