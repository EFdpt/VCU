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
#include "model.h"

int AIR = 0;
int RTD = 0;
int BSPD = 1;

int SCthr=600;
int dinamica1 = 885;  //75% di th1Up-th1Low utilizzato per mappa pedale non lineare
int dinamica2 = 944; //80% della corsa del pedale

int RunTH = 3195; //25% della corsa del pedale
int RunTH5 = 2959;
int RunBK = 10; //10% della corsa del freno
int RTDBK = 10; //pressione freno per RTD (10%)

void setup() {
  model_init();
}


/*
 * stato 0, accensione della vettura
 */
void STAND() {
  while (!BSPD);
  BSPD = 1;
  //AIR = 0;
  RTD = 0;
  digitalWrite(PRE, LOW);
  digitalWrite(AIRGnd, LOW);
  digitalWrite(AIRcc, LOW);
  /*lettura bottone (chiuso = GND) e SC maggiore di 1.28V
    transizione per stato HVON
  */
  if (digitalRead(AIRB) == LOW && get_SC_value() > SCthr) {
    digitalWrite(PRE, HIGH);
    digitalWrite(AIRGnd, HIGH);
    delay(3000); // TODO: fare senza delay
    digitalWrite(AIRcc, HIGH);
    digitalWrite(PRE, LOW);
    //if(ricevuto pacchetto AIR chiusi){
    //AIR = 1;
    setState(HVON);
    //}else AIR=0
  }
}

/*
 * stato 1, alta tensione attiva
 */
void HVON() {

  //RTD = 0;

  /*transizione verso stato DRIVE
    Il segnale dello SC è maggiore di 1,28 V
    suono RTD 2 secondi
  */
  if (get_SC_value() > SCthr && get_brake_percentage() > RTDBK && digitalRead(RTDB) == LOW) { //brake >800

  //buzzer che suona
    for (int count = 0; count < 4; count++) {
        delay(100);
        digitalWrite(BUZZ, HIGH);
        delay(270);
        digitalWrite(BUZZ, LOW);
    }
    
    //if(feedback RTD){
    //RTD = 1;
    setState(DRIVE);
    //}else RTD=0
    }
  /*ritorno allo stato STAND*/
    if (get_SC_value() < SCthr)  setState(STAND);
}



void DRIVE() {
  RTD = 1;
  AIR = 1;

  /*
    il while permette un ciclo più veloce evitando di
    ripetere la chiamata alla funzione di stato ad ogni richiesta di coppia
    appena scattano le plausibilità si esce dal while di guida plausibile
  */
    while (get_SC_value() > SCthr && RTD && get_apps_plausibility() && get_brake_plausibility()) {

    /*drive!!*/
        if (get_apps_plausibility() && get_brake_plausibility()) {  
//      //mappa spezzata su richiesta coppia
//      if (th1 <= dinamica1 + th1Low)
//        analogWrite(DAC1, map(th1, th1Low, dinamica2 + th1Low, 0, 2047));
//      else
//        analogWrite(DAC1, map(th1, dinamica2 + th1Low, th1Up, 2048, 4095));

            if (bk<3500) {  //3500=2.82V IN
                inverter_torque_request(map(get_tps1_percentage(), 0, 100, INVERTER_TORQUE_MIN, INVERTER_TORQUE_MAX)); // regen
                analogWrite(BRAKE_REGEN_PIN, 0);
            } else {
                inverter_torque_request(0);
                analogWrite(BRAKE_REGEN_PIN, 1613);  //1296=2.6V OUT
            }

            Serial.print("TH1: "); Serial.print(get_tps1_percentage()); Serial.print("   BREAK: "); Serial.println(get_brake_percentage());
        }
    }

  /*ritorno allo stato STAND*/
    if (get_SC_value() < SCthr)  setState(STAND);

  /*scatto plausibilità TPS o scollegamento BRAKE*/
    if (!get_apps_plausibility() || bk < 500) {
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
    if (get_SC_value() < SCthr)  setState(STAND);
}

/* 
 * errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
 */
void NOTDRIVE() {
    if (get_SC_value() < SCthr) setState(STAND);
  /*
    th1-th2 <= Upper bound && th1-th2 >= Lower bound && bk < 10%
   **/
    if (get_apps_plausibility() && bk < RunBK) {
        setState(DRIVE);
        Serial.println("stato corrente 2");
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
