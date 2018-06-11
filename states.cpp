#include "states.h"
#include "can_funzionale.h"
#include "can_servizi.h"
#include "model.h"
#include "common.h"

#define SC_THRES    600
#define RunBK       10 //10% della corsa del freno
#define RTDBK       10 //pressione freno per RTD (10%)
#define REGEN_BK_PERCENTAGE         40

volatile bool RTD = false;

volatile    e_nodeState current_state   = STAND;
uint8_t     nodeId                      = VCU_NODE_ID;

__attribute__((__inline__)) e_nodeState getState() {
	return current_state;
}

__attribute__((__inline__)) void setState(e_nodeState newState) {
	current_state = newState;
}

__attribute__((__inline__)) uint8_t getNodeId() {
	return nodeId;
}

__attribute__((__inline__)) void setNodeId(uint8_t nodeId) {
	nodeId = nodeId;
}

/*
 * stato 0, accensione della vettura
 */
void stand() {
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

    /*transizione verso stato DRIVE
        Il segnale dello SC è maggiore di 1,28 V
        suono RTD 2 secondi
    */
    if (get_SC_value() > SC_THRES && get_brake_percentage() > RTDBK && digitalRead(RTDB) == LOW) { //brake >800

        //buzzer che suona
        for (int count = 0; count < 4; count++) {
            delay(100);
            digitalWrite(BUZZ, HIGH);
            delay(270);
            digitalWrite(BUZZ, LOW);
        }
        
        RTD = true;
        setState(DRIVE);
    }
    
    /*ritorno allo stato STAND*/
    if (get_SC_value() < SC_THRES)  setState(STAND);
}

void drive() {
    while (get_SC_value() > SC_THRES && RTD && get_apps_plausibility() && get_brake_plausibility()) {

        if (get_apps_plausibility() && get_brake_plausibility()) {
            if (get_brake_percentage() < REGEN_BK_PERCENTAGE) {  // 2.82V IN
                volatile uint16_t torque_request = get_tcs_torque_coefficient() * get_tps1_percentage();
                inverter_torque_request(torque_request);
                analogWrite(BRAKE_REGEN_PIN, 0); // regen OFF
            } else {
                inverter_torque_request(0);
                analogWrite(BRAKE_REGEN_PIN, 1613);  //1296=2.6V OUT - regen ON
            }

            Serial.print("TPS1: "); Serial.print(get_tps1_percentage()); Serial.print("   BRAKE: "); Serial.println(get_brake_percentage());
        }
    }

  /*ritorno allo stato STAND*/
    if (get_SC_value() < SC_THRES)  setState(STAND);

  /*scatto plausibilità TPS*/
    if (!get_apps_plausibility()) {
        inverter_torque_request(0);
        analogWrite(BRAKE_REGEN_PIN, 0);
        setState(NOTDRIVE);
    }


  /* scatto plausibilità pressione congiunta ACCELERATORE e FRENO
     ciclo di rientro pedale < 5% della corsa
  */
    while (!get_brake_plausibility())
        Serial.println("   FRENO + ACCELERATORE !!!");

  /*ritorno allo stato STAND*/
    if (get_SC_value() < SC_THRES)  setState(STAND);
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