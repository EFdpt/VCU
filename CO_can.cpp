#include "CO_can.h"
#include "can_servizi.h"
#include "can_funzionale.h"

bool can_init() {
    if (!can_funzionale_initialized())
    	can_funzionale_init();
    if (!can_servizi_initialized())
        return can_servizi_init();
    return true;
}