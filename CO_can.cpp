#include "CO_can.h"
#include "can_servizi.h"
#include "can_funzionale.h"

void can_init() {
    can_funzionale_init();
    can_servizi_init();
}