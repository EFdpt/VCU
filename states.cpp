/** 
 *  @file           states.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          FSM implementation file
 */

#include "states.h"
#include "can_funzionale.h"
#include "can_servizi.h"
#include "rtds.h"
#include "model.h"
#include "common.h"

/**
 *  @addtogroup stages_group
 *   @{
 */

/**
 *  @def    SC_THRES
 *  @brief  SC voltage minimum value
 */
#define SC_THRES    600

/**
 *  @def    RunBK
 *  @brief  Brake pedal position percentage threshold to pass from 
 *          NOTDRIVE to DRIVE 
 */
#define RunBK       10

/**
 *  @def    RTDBK
 *  @brief  Brake pedal position percentage threshold to pass from 
 *          HVON to DRIVE 
 */
#define RTDBK       10

/**
 *  @def    REGEN_BK_PERCENTAGE
 *  @brief  Brake pedal position percentage threshold to active regen request
 */
#define REGEN_BK_PERCENTAGE         40

/**
 *  @var    volatile bool RTD;
 *  @brief  Ready To Drive flag
 */
volatile bool RTD = false;

/**
 *  @var    volatile e_nodeState current_state;
 *  @brief  Current state on the FSM
 */
volatile    e_nodeState current_state   = STAND;

/**
 *  @brief      Return current state on the @ref FSM_page
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @return     current state on the FSM
 */
__attribute__((__inline__))
e_nodeState getState() {
	return current_state;
}


/**
 *  @brief      Set current state on the @ref FSM_page
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @param[in]  newState    New state transition
 */
__attribute__((__inline__))
void setState(e_nodeState newState) {
	current_state = newState;
}

/**
 *  @brief      @ref STAND state task on the FSM: ignition of the car.
 *              If AIR button is pressed and SC voltage value is greater than
 *              #SC_THRES then current state passes to @ref HVON, activating Precharge,
 *              AIR- and AIR+.
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 */
void stand() {
    // model_enable_calibrations();
    RTD = false;
    digitalWrite(PRE, LOW);
    digitalWrite(AIRGnd, LOW);
    digitalWrite(AIRcc, LOW);
    
    if (digitalRead(AIRB) == LOW && get_SC_value() > SC_THRES) {
        digitalWrite(PRE, HIGH);
        digitalWrite(AIRGnd, HIGH);
        delay(3000);
        digitalWrite(AIRcc, HIGH);
        digitalWrite(PRE, LOW);
        setState(HVON);
    }
}

/**
 *  @brief      @ref HVON state task on the FSM: active high voltage.
 *  
 *              If RTD button is pressed, SC voltage value is greater than
 *              #SC_THRES and brake pedal position percentage is greater than
 *              #RTDBK then current state passes to @ref DRIVE, triggering @ref RTDS.
 *
 *              If SC voltage value is lower than #SC_THRES, (SC undervoltage error),
 *              then current state passes to @ref STAND.
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 */
void hvon() {

    /* transizione verso stato DRIVE
        Il segnale dello SC è maggiore di SCmin
        suono RTD 2 secondi
    */
    if (get_SC_value() > SC_THRES && get_brake_percentage() > RTDBK && digitalRead(RTDB) == LOW) {

        ready_to_drive_sound_start();
        
        RTD = true;
        // model_disable_calibrations();
        setState(DRIVE);
    }
    
    /*ritorno allo stato STAND*/
    if (get_SC_value() < SC_THRES)  setState(STAND);
}

/**
 *  @brief      @ref DRIVE state task on the @ref FSM_page.
 *  
 *              If pedals values are consistent (no implausibility) and SC voltage 
 *              value is greater than #SC_THRES then torque or regen (if vehicle 
 *              speed is \f$>\ 5\ km/h\f$) request is sent to inverter; request to
 *              inverter is done through @ref CAN_funzionale_page if online or through analog 
 *              signals.
 *              
 *              If SC voltage value is lower than #SC_THRES, (SC undervoltage error),
 *              then current state passes to STAND.
 *
 *              If there are pedals implausibility then current state passes to
 *              NOT DRIVE state.
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 */
void drive() {
    while (get_SC_value() > SC_THRES && RTD && get_apps_plausibility() && get_brake_plausibility()) {

        volatile uint16_t apps_percentage;
        if (get_apps_plausibility() && get_brake_plausibility()) {
            if (get_brake_percentage() < REGEN_BK_PERCENTAGE) {  // 2.82V IN
                apps_percentage = (get_tps1_percentage() + get_tps2_percentage()) / 2;
                volatile uint16_t torque_request = get_tcs_torque_coefficient() * apps_percentage;
                inverter_torque_request(torque_request);
                inverter_regen_request(0);
            } else {
                inverter_torque_request(0);
                inverter_regen_request(1613);
            }

            Serial.print("APPS: "); Serial.print(apps_percentage); Serial.print("   BRAKE: "); Serial.println(get_brake_percentage());
        }
    }

  /*ritorno allo stato STAND*/
    if (get_SC_value() < SC_THRES)  setState(STAND);

  /*scatto plausibilità TPS*/
    if (!get_apps_plausibility()) {
        inverter_torque_request(0);
        // analogWrite(BRAKE_REGEN_PIN, 0);
        setState(NOTDRIVE);
    }


  /* scatto plausibilità pressione congiunta ACCELERATORE e FRENO
     ciclo di rientro pedale < 5% della corsa
  */
    while (!get_brake_plausibility())
        Serial.println("   FRENO + ACCELERATORE !!!");
}

/**
 *  @brief      NOT DRIVE state task on the FSM: inconsistent pedals values.
 *  
 *              If SC voltage value is lower than #SC_THRES, (SC undervoltage error),
 *              then current state passes to STAND state.
 *
 *              If pedals values return consistent (i.e. plausibility check verified),
 *              then current state passes to DRIVE state.
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 */
void notdrive() {
    if (get_SC_value() < SC_THRES) setState(STAND);

    if (get_apps_plausibility() && get_brake_percentage() < RunBK) {
        RTD = true;
        setState(DRIVE);
    }
}

/**
 *  @var    void(*state_table[])(void);
 *  @brief  FSM tasks function pointers
 */
void(*state_table[])(void) = {stand, hvon, drive, notdrive};

/**
 *  @brief      Execute current state task
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 */
void state_dispatch() {
    state_table[current_state]();
}

/**
 *  @}
 */