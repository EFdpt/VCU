#ifndef _CO_CAN_H
#define _CO_CAN_H

#include <stdint.h>

typedef struct {
    uint16_t 	cob_id; /**< message's ID */
    uint8_t 	len; /**< message's length (0 to 8) */
    uint8_t 	data[8]; /**< message's datas */
} Message;

#define Message_Initializer {0,0,{0,0,0,0,0,0,0,0}}

void canSend(Message* m);

void initCAN();

#endif /* _CO_CAN_H_ */