/* questo codice sviluppato da Valerio Dodet è dedicato al funzionamento di un arduino DUE
 * che svolga la funzione di unità di controllo della vettura FastCharge per il team
 * Fast Charge in Formula SAE
 *  
 *  README: https://goo.gl/wGvLdF
 *  SITE: https://www.facebook.com/SapienzaFastCharge
 *    
 * stati
 * STAND (Initialisation):  stato 0, accensione della vettura, si ritorna qui ogni volta che casca l'SC
 *
 * HVON (Pre-operational): stato 1, alta tensione attiva
 *    si accede solo da STAND tramite AIRbutton e SC>3V
 *
 * DRIVE (Operational):stato 2, lo stato di guida sicura, accedibile tramite procedura RTD ma anche con lo 
 *    scatto delle plausibilità tramite procedura di rientro
 *
 * NOTDRIVE (Stopped): stato 3, errore con la pedaliera, i sensori dei pedali sono scollegati o fuori
 *        range. Disabilitate richiesta di coppia e richiesta di frenata, si entra solo
 *        tramite scatto delle plausibilità o assenza del freno.
 *
 */

#include "common.h"
#include "CAN_ID.h"
#include "CO_can.h"
#include "model.h"

volatile bool RTD = false;

#define SC_THRES    600
int dinamica1     = 885;  //75% di th1Up-th1Low utilizzato per mappa pedale non lineare
int dinamica2     = 944; //80% della corsa del pedale

#define RunBK       10 //10% della corsa del freno
#define RTDBK       10 //pressione freno per RTD (10%)
#define REGEN_BK_PERCENTAGE         40

void setup() {
    while (!can_init()) {};
    model_init();
}

/*
 * stato 0, accensione della vettura
 */
void STAND() {
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
void HVON() {

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

void DRIVE() {
    // RTD = true; // serve risettarlo a true???

  /*
    il while permette un ciclo più veloce evitando di
    ripetere la chiamata alla funzione di stato ad ogni richiesta di coppia
    appena scattano le plausibilità si esce dal while di guida plausibile
  */
    while (get_SC_value() > SC_THRES && RTD && get_apps_plausibility() && get_brake_plausibility()) {

    /*drive!!*/
        if (get_apps_plausibility() && get_brake_plausibility()) {
            if (get_brake_percentage() < REGEN_BK_PERCENTAGE) {  // 2.82V IN
                inverter_torque_request(map(get_tps1_percentage(), 0, 100, INVERTER_TORQUE_MIN, INVERTER_TORQUE_MAX));
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

  /*scatto plausibilità TPS o scollegamento BRAKE*/
    if (!get_apps_plausibility() || get_brake_percentage() < 5) {
        inverter_torque_request(0);
        analogWrite(BRAKE_REGEN_PIN, 0);
        setState(NOTDRIVE);
    }


  /* scatto plausibilità pressione congiunta ACCELERATORE e FRENO
     ciclo di rientro pedale < 5% della corsa
  */
    while (!get_brake_plausibility()) {
        Serial.println("   FRENO + ACCELERATORE !!!");
      //      //BYPASS BSPD
      //      if (media(bk) > 870) {
      //        digitalWrite(AIRGnd, LOW);
      //        digitalWrite(AIRcc, LOW);
      //        BSPD = 0;
      //        current_state = 0;
      //      }
      //      //END BYPASS BSPD
    }

  /*ritorno allo stato STAND*/
    if (get_SC_value() < SC_THRES)  setState(STAND);
}

/* 
 * errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
 */
void NOTDRIVE() {
    if (get_SC_value() < SC_THRES) setState(STAND);

    if (get_apps_plausibility() && get_brake_percentage() < RunBK) {
        RTD = true;
        setState(DRIVE);
        Serial.println("stato corrente DRIVE");
    }
}

/* Tabella dei possibili stati:
  la tabella punta alle funzioni in elenco (stati)
*/
void(*state_table[])(void) = {STAND, HVON, DRIVE, NOTDRIVE};

void loop() {
    state_table[current_state]();
    /*
    currMillis2 = millis();

  if (currMillis2 - prevMillis2 >= plaus1Millis2) {
    prevMillis2 = currMillis2;
    Serial.print("SC= "); Serial.print(analogRead(SC)); Serial.print("  ");
    Serial.print("Stato corrente: "); Serial.print(current_state); Serial.print("  TPS1: "); Serial.print(analogRead(TPS1));
    Serial.print("    TPS2: "); Serial.print(analogRead(TPS2)); Serial.print("    Brake IN: "); Serial.println(analogRead(BRAKEIN));
  }
  */
}
