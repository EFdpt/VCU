#include "states.h"
#include "can_funzionale.h"
#include "can_servizi.h"
#include "rtds.h"
#include "model.h"
#include "common.h"

#define SC_THRES    600
#define RunBK       10 //10% della corsa del freno
#define RTDBK       10 //pressione freno per RTD (10%)
#define REGEN_BK_PERCENTAGE         40

volatile bool RTD = false; // Ready To Drive

volatile    e_nodeState current_state   = STAND;

__attribute__((__inline__)) e_nodeState getState() {
	return current_state;
}

__attribute__((__inline__)) void setState(e_nodeState newState) {
	current_state = newState;
}

/*
 * stato 0, accensione della vettura
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

/*
 * stato 1, alta tensione attiva
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

void drive() {
    while (get_SC_value() > SC_THRES && RTD && get_apps_plausibility() && get_brake_plausibility()) {

        volatile uint16_t apps_percentage;
        if (get_apps_plausibility() && get_brake_plausibility()) {
            if (get_brake_percentage() < REGEN_BK_PERCENTAGE) {  // 2.82V IN
                apps_percentage = (get_tps1_percentage() + get_tps2_percentage()) / 2;
                volatile uint16_t torque_request = get_tcs_torque_coefficient() * apps_percentage;
                inverter_torque_request(torque_request);
                // analogWrite(BRAKE_REGEN_PIN, 0); // regen OFF
            } else {
                inverter_torque_request(0);
                // analogWrite(BRAKE_REGEN_PIN, 1613);  // 1296=2.6V OUT - regen ON
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

/* 
 * errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
 */
void notdrive() {
    if (get_SC_value() < SC_THRES) setState(STAND);

    if (get_apps_plausibility() && get_brake_percentage() < RunBK) {
        RTD = true;
        setState(DRIVE);
    }
}

/* Tabella dei possibili stati:
  la tabella punta alle funzioni in elenco (stati)
*/
void(*state_table[])(void) = {stand, hvon, drive, notdrive};

void state_dispatch() {
    state_table[current_state]();
}