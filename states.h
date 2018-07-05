#ifndef _STATES_H_
#define _STATES_H_

#include "CO_can.h"

enum enum_nodeState {
    STAND = 0,
    HVON,
    DRIVE,
    NOTDRIVE,
    MAX_STATES
} __attribute__ ((packed));

typedef enum enum_nodeState e_nodeState;

e_nodeState getState();

void setState(e_nodeState newState);

uint8_t getNodeId();

void setNodeId(uint8_t nodeId);

void state_dispatch();

#endif /* _STATES_H_ */