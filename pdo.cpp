#include "pdo.h"
#include "states.h"

#include "model.h"

void buildPDO(uint8_t PDOtype, Message* pdo) {
    pdo -> cob_id = (SET_FUNC_CODE(PDOtype) | getNodeId());

    switch (PDOtype) {

#if defined(_FRONTAL_)
        case PDO1tx:
            pdo -> len = 4;
            pdo -> data[0] = tps1_percentage;
            pdo -> data[1] = tps2_percentage;
            pdo -> data[2] = brake_percentage;
            pdo -> data[3] = (uint8_t) (apps_plausibility ? 0xF0 : 0x00) | (brake_plausibility ? 0x0F : 0x00);
            break;
        case PDO2tx:
            pdo -> len = 6;
            ((uint16_t*) pdo -> data)[0] = get_fr_dx_rpm();
            ((uint16_t*) pdo -> data)[1] = get_fr_sx_rpm();
            pdo -> data[4] = fr_dx_susp;
            pdo -> data[5] = fr_sx_susp;
            break;
#elif defined(_RETRO_)

        case PDO1tx:
            pdo -> len = 8;
            ((uint16_t*) pdo -> data)[0] = get_rt_dx_rpm();
            ((uint16_t*) pdo -> data)[1] = get_rt_sx_rpm();
            pdo -> data[4] = acc_x_value;
            pdo -> data[5] = acc_z_value;
            pdo -> data[6] = rt_dx_susp;
            pdo -> data[7] = rt_sx_susp;
            break;
#endif
        default: { }
    }
}

#if defined(_RETRO_)
void proceedPDO(Message* m) {
    switch (GET_FUNC_CODE(m -> cob_id)) {
        case PDO1rx: // first PDO from frontal SCU (pedals)
            tps1_percentage = (uint8_t) m -> data[0];
            tps2_percentage = (uint8_t) m -> data[1];
            brake_percentage = (uint8_t) m -> data[2];
            apps_plausibility = (m -> data[3] & 0xF0) ? true : false;
            brake_plausibility = (m -> data[3] & 0x0F) ? true : false;
            break;
        case PDO2rx: // second PDO from frontal SCU (suspensions & wheels)
            fr_sx_rpm = (uint16_t) ((uint16_t*) m -> data)[0];
            fr_dx_rpm = (uint16_t) ((uint16_t*) m -> data)[1];
            fr_sx_susp = (uint8_t) m -> data[4];
            fr_dx_susp = (uint8_t) m -> data[5];
            break;
        default: {}
    }
}
#endif