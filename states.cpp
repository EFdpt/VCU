#include "timer.h"
#include "def.h"
#include "pdo.h"
#include "states.h"
#include "nmt.h"
#include "CO_can.h"

#include "common.h"
#include "model.h"

#if defined(_RETRO_)
#include "radio.h"
#endif

e_nodeState current_state = Initialisation;
uint8_t 	nodeId;

__attribute__((__inline__)) e_nodeState getState() {
	return current_state;
}

__attribute__((__inline__)) void setState(e_nodeState newState) {
	current_state = newState;
}

__attribute__((__inline__)) uint8_t getNodeId() {
	return nodeId;
}

__attribute__((__inline__)) void setNodeId(uint8_t nodeId) {
	nodeId = nodeId;
}

// gestisci solo comandi NMT e se SCU_R anche i PDO di SCU_F
void canDispatch(Message* m) {
	switch (GET_FUNC_CODE(m -> cob_id)) {
		case NMT:
            proceedNMTstateChange(m);
            break;
#if defined(_RETRO_)
        case PDO1rx:
        case PDO2rx:
            proceedPDO(m);
            break;
#endif
        default: {}
	}
}

__attribute__((__inline__)) void initialisation() {
    setState(Initialisation);
    model_init();
#if defined(_FRONTAL_)
    model_enable_calibrations();
#elif defined(_RETRO_)
    radio_init();
#endif
    initCAN();
    timerInit();
}

__attribute__((__inline__)) void preOperational() {
    setState(Pre_operational);
}

__attribute__((__inline__)) void operational() {
    setState(Operational);
	timerStart();
}

__attribute__((__inline__)) void stopped() {
    setState(Stopped);
	timerStop();
}