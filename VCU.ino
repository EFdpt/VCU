/* README: https://goo.gl/wGvLdF
 *  SITE: https://www.facebook.com/SapienzaFastCharge
 *    
 * stati
 * STAND:  stato 0, accensione della vettura, si ritorna qui ogni volta che casca l'SC
 *
 * HVON: stato 1, alta tensione attiva
 *    si accede solo da STAND tramite AIRbutton e SC>SCmin
 *
 * DRIVE: stato 2, lo stato di guida sicura, accedibile tramite procedura RTD ma anche con lo 
 *    scatto delle plausibilità tramite procedura di rientro
 *
 * NOTDRIVE: stato 3, errore con la pedaliera, i sensori dei pedali sono scollegati o fuori
 *        range. Disabilitate richiesta di coppia e richiesta di frenata, si entra solo
 *        tramite scatto delle plausibilità o assenza del freno.
 *
 */

#include "common.h"
#include "CO_can.h"
#include "can_servizi.h"
#include "model.h"
#include "states.h"

void setup() {
    volatile uint16_t timeout = 2000;

    while (!can_init()) // initialize CAN bus baudrate & configure mailboxes
        if (timeout--) break;

    model_init();

    if (can_servizi_online())
        can_servizi_go_operational();
}

void loop() {
    state_dispatch();
}
