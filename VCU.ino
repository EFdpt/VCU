/** 
 *  @file           VCU.ino
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          Main module file
 */

#include "common.h"
#include "CO_can.h"
#include "can_servizi.h"
#include "model.h"
#include "states.h"

/**
 *  @mainpage FastChargeSAE VCU firmware
 *  
 *  Vehicle Control Unit (VCU) firmware is based upon 4 states:
 *  - STAND: stato 0, accensione della vettura, si ritorna qui ogni volta che
 *              casca l'SC
 *  - HVON: stato 1, alta tensione attiva si accede solo da STAND tramite AIRbutton 
 *              e SC>3V
 *  - DRIVE: stato 2, lo stato di guida sicura, accedibile tramite procedura RTD 
 *              ma anche con lo scatto delle plausibilità tramite procedura di rientro
 */

/**
 *  @brief      This function perform basic board setup.
 *              -   It starts initializing both CAN funzionale (with inverter) and
 *                  CAN servizi (with the two SCUs and TCS); if the comunication 
 *                  between inverter and VCU can't be established via CAN bus
 *                  then the VCU is configured to request torque value to inverter
 *                  by analog signal.
 *
 *              -   It initializes board hardware (TODO: see model)
 *              
 *              -   If the configuration over CAN servizi with the frontal SCU was
 *                  successful then VCU (master) send an NMT request to go in
 *                  'Operational' state (TODO: see later).
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void setup() {
    volatile uint16_t timeout = 2000;

    while (!can_init()) // initialize CAN bus baudrate & configure mailboxes
        if (timeout--) break;

    model_init();

    if (can_servizi_online())
        can_servizi_go_operational();
}

/**
 *  @brief      This function is called into endless while main loop.
 *              It takes care of dispatching states of the finite state machine
 *              (TODO: see states)
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
__attribute__((__inline__))
void loop() {
    state_dispatch();
}
