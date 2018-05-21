#ifndef _STATES_H_
#define _STATES_H_

#include "CO_can.h"

enum CO_nodeState {
    Initialisation = 0x00,
    Disconnected = 0x01,
    Connecting = 0x02,
    Preparing = 0x02,
    Stopped = 0x04,
    Operational = 0x05,
    Pre_operational = 0x7F,
    Unknown_state = 0x0F
} __attribute__ ((packed));

typedef enum CO_nodeState CO_nodeState;

typedef enum e_nodeState_ {
    STAND       = 0x00,
    HVON        = 0x01,
    DRIVE       = 0x02,
    NOTDRIVE    = 0x03
} e_nodeState;

/** 
 * @brief Called by driver/app when receiving messages
 * @param *d Pointer on a CAN object data structure
 * @param *m Pointer on a CAN message structure
 */
void canDispatch(Message* m);

e_nodeState getState();

void setState(e_nodeState newState);

uint8_t getNodeId();

void setNodeId(uint8_t nodeId);

#endif /* _STATES_H_ */