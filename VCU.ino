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
 *  VCU is an embedded system employeed to control the functional operations of the vehicle.
 *  It takes care of the management of the state of drive according to the @ref FSM_page
 *  section.
 */

/**
 *  @defgroup Main_group_module Main module
 *   @{
 */

/**
 *  @brief      This function perform basic board setup.
 *              -   It starts initializing both CAN funzionale (with inverter) and
 *                  CAN servizi (with the two SCUs and TCS); if the comunication 
 *                  between inverter and VCU can't be established via CAN bus
 *                  then the VCU is configured to request torque value to inverter
 *                  by analog signal.
 *
 *              -   It initializes board hardware (@ref Board_model_group)
 *              
 *              -   If the configuration over CAN servizi with the frontal SCU was
 *                  successful then VCU (master) send an NMT request to go in
 *                  'Operational' state.
 *
 *  @image html setup_activity.svg
 *  @image latex setup_activity.eps "Setup Activity" width=10cm
 *  
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void setup() {
    volatile uint16_t timeout = 100;

    while (!can_init()) // initialize CAN bus baudrate & configure mailboxes
        if (timeout--) break;

    model_init();

    if (can_servizi_online())
        can_servizi_go_operational();
}

/**
 *  @brief      This function is called into endless while main loop.
 *              It takes care of dispatching states of the @ref FSM_page
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
__attribute__((__inline__))
void loop() {
    state_dispatch();
}

/**
 *  @}
 */
