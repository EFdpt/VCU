#include "can_servizi.h"

#include <due_can.h>
#include <DueTimer.h>

volatile bool can_serv_initialized  = false;
volatile bool SCU_F_online          = false;
volatile bool TCS_online            = false;

volatile uint32_t curr_seq_num       = 0;
volatile uint32_t next_seq_num       = 0;

// CAN acquisition
volatile uint8_t    tps1_percentage    = 0;
volatile uint8_t    tps2_percentage    = 0;
volatile uint8_t    brake_percentage   = 0;
volatile bool       apps_plausibility  = true;
volatile bool       brake_plausibility = true;

volatile uint8_t    tcs_coefficient = 0;

void timeout() {
    if (next_seq_num > curr_seq_num) {
        SCU_F_online = true;
        curr_seq_num = next_seq_num;
    } else
        SCU_F_online = false;
}

__attribute__((__inline__))
volatile bool can_servizi_initialized() {
    return can_serv_initialized;
}

void CAN_SERV_BOOTUP_CB(CAN_FRAME* frame) {
    if (frame -> id == 0x700 + SCU_FRONTAL_NODE_ID)
        SCU_F_online = true;
    else if (frame -> id == 0x700 + TCU_NODE_ID)
        TCS_online = true;
}

void CAN_SERV_GENERAL_CB(CAN_FRAME* frame) {
    switch (frame -> id) {
        case (0x180 + SCU_FRONTAL_NODE_ID): // (pedals)
            tps1_percentage = (uint8_t) frame -> data.byte[0];
            tps2_percentage = (uint8_t) frame -> data.byte[1];
            brake_percentage = (uint8_t) frame -> data.byte[2];
            apps_plausibility = (frame -> data.byte[3] & 0xF0) ? true : false;
            brake_plausibility = (frame -> data.byte[3] & 0x0F) ? true : false;

            // fault check
            next_seq_num++; // received new frame
            break;
        case (0x180 + TCU_NODE_ID): // TCU coefficient limiter
            tcs_coefficient = (uint8_t) frame -> data.byte[0];
            break;
        default: {}
    }
}

bool can_servizi_init() {
    if (!CAN_SERVIZI.begin(CAN_SERV_BAUDRATE))
        return (can_serv_initialized = false);
  
    uint32_t i;

    // set mailbox 0 for receiving boot-up frames
    CAN_SERVIZI.setRXFilter(0, 0x00000700, 0x1FFFFF80, false);
    CAN_SERVIZI.setCallback(0, CAN_SERV_BOOTUP_CB);

    // set remaining mailboxes for receiving TPDOs
    for (i = 2; i < 8; i++) {
        CAN_SERVIZI.setRXFilter(i, 0x00000080, 0x1FFFFC80, false);
        CAN_SERVIZI.setCallback(i, CAN_SERV_GENERAL_CB);
    }

    DueTimer::getAvailable().attachInterrupt(timeout).start(CAN_SERVIZI_TIMEOUT_PERIOD);

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
}

__attribute__((__inline__))
volatile bool can_servizi_online() {
    return (can_serv_initialized && SCU_F_online);
}

__attribute__((__inline__))
volatile bool tcs_online() {
    return (can_serv_initialized && TCS_online);
}

__attribute__((__inline__)) volatile uint8_t get_servizi_tps1() {
    return tps1_percentage;
}

__attribute__((__inline__)) volatile uint8_t get_servizi_tps2() {
    return tps2_percentage;
}

__attribute__((__inline__)) volatile uint8_t get_servizi_brake() {
    return brake_percentage;
}

__attribute__((__inline__)) volatile bool    get_servizi_apps_plausibility() {
    return apps_plausibility;
}

__attribute__((__inline__)) volatile bool    get_servizi_brake_plausibility() {
    return brake_plausibility;
}

__attribute__((__inline__))
volatile uint8_t get_tcs_torque_coefficient() {
    return TCS_online ? tcs_coefficient : 100;
}