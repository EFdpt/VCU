#include "can_servizi.h"

volatile bool SCU_F_online          = false;
volatile bool SCU_R_online          = false;
volatile bool TCS_online            = false;

// CAN acquisition
volatile uint8_t    tps1_percentage    = 0;
volatile uint8_t    tps2_percentage    = 0;
volatile uint8_t    brake_percentage   = 0;
volatile bool       apps_plausibility  = true;
volatile bool       brake_plausibility = true;

void CAN_SERV_GENERAL_CB(CAN_FRAME* frame) {

}

bool can_servizi_init() {
    if (!CAN_SERVIZI.begin(CAN_SERV_BAUDRATE))
        return false;
  
    uint32_t i = 0;

    for (; i < 8; i++) {
        CAN_SERVIZI.setRXFilter(i, 0, 0, false);
        CAN_SERVIZI.setCallback(i, CAN_SERV_GENERAL_CB);
    }

    return true;
}

__attribute__((__inline__))
volatile bool can_servizi_online() {
    return SCU_F_online;
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