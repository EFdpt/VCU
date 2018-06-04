#include "CO_can.h"
#include "can_servizi.h"
#include "can_funzionale.h"

bool can_init() {
    return (can_funzionale_online() ? true : can_funzionale_init() && 
        can_servizi_online() ? true : can_servizi_init());
}