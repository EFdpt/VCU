#include <DueTimer.h>

#include "common.h"
#include "timer.h"
#include "pdo.h"
#include "CO_can.h"

#if defined(_RETRO_)
    #include "radio.h"
#endif

#define SCU_FRONT_FIRST_SLOT 			0
#define SCU_FRONT_SECOND_SLOT 			1
#define SCU_REAR_SLOT 					2
#define TCS_SLOT 						3

#define TIME_SLOT_MASK                  3
#define RADIO_SLOT_MASK                 7 // 3 slot temporali di _RETRO_ (spedisci via radio ogni TIME_SLOT_PERIOD * 4 * 3, con 4 = #slot di attesa per SCU_RETRO)

volatile uint8_t		t_slot = SCU_FRONT_FIRST_SLOT;

#if defined(_RETRO_)
volatile uint8_t        radio_slot = 0;
#endif

DueTimer* timer;

void TimeDispatch() { // send PDOs periodically
#if defined(_FRONTAL_)
    switch (t_slot) {
		case SCU_FRONT_FIRST_SLOT: {
			Message m = Message_Initializer;
            buildPDO(PDO1tx, &m);
            canSend(&m);
			break;
        }
		case SCU_FRONT_SECOND_SLOT: {
            Message m = Message_Initializer;
            buildPDO(PDO2tx, &m);
            canSend(&m);
			break;
        }
        default: {}
    }
#elif defined(_RETRO_)
    switch (t_slot) {
		case SCU_REAR_SLOT: {
            Message m = Message_Initializer;
            buildPDO(PDO1tx, &m);
            canSend(&m);
            if (radio_slot)
                radio_transmit = true;
            radio_slot = (radio_slot + 1) & RADIO_SLOT_MASK;
			break;
        }
        default: {}
    }
#endif

    t_slot = (t_slot + 1) & TIME_SLOT_MASK;
}

__attribute__((__inline__)) void timerInit() {
	TIMER.attachInterrupt(TimeDispatch);
}

__attribute__((__inline__)) void timerStart() {
	TIMER.start(TIME_SLOT_PERIOD);
}

__attribute__((__inline__)) void timerStop() {
	TIMER.stop();
}
