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
int th1 = 0;
int th2 = 0;
int bk = 0;
int plaus2 = 0;
int plaus1 = 0;
int BSPD = 1;

int SCthr=600;
int dinamica1 = 885;  //75% di th1Up-th1Low utilizzato per mappa pedale non lineare
int dinamica2 = 944; //80% della corsa del pedale

int RunTH = 3195; //25% della corsa del pedale
int RunTH5 = 2959;
int RunBK = 10; //10% della corsa del freno
int RTDBK = 10; //pressione freno per RTD (10%)

void setup() {
  // put your setup code here, to run once:
  pinMode(FAN, OUTPUT);
  pinMode(AIRcc, OUTPUT);
  pinMode(AIRGnd, OUTPUT);
  pinMode(PRE, OUTPUT);
  pinMode(BUZZ, OUTPUT);

  pinMode(BRAKEIN, INPUT);
  pinMode(AIRB, INPUT_PULLUP);
  pinMode(RTDB, INPUT_PULLUP);

  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial);

  //azzeramento TORQUE OUT and BRAKE OUT
  analogWrite(DAC0, 0); //brake
}


/*
 * stato 0, accensione della vettura
 */
void STAND() {
  //invio pacchetto VCU_OK su CAN
  while (!BSPD);
  BSPD = 1;
  AIR = 0;
  RTD = 0;
  plaus2 = 0;
  plaus1 = 0;
  digitalWrite(PRE, LOW);
  digitalWrite(AIRGnd, LOW);
  digitalWrite(AIRcc, LOW);
  /*lettura bottone (chiuso = GND) e SC maggiore di 1.28V
    transizione per stato HVON
  */
  if (digitalRead(AIRB) == LOW && analogRead(SC) > SCthr) {
    digitalWrite(PRE, HIGH);
    digitalWrite(AIRGnd, HIGH);
    delay(3000);
    digitalWrite(AIRcc, HIGH);
    digitalWrite(PRE, LOW);
    //if(ricevuto pacchetto AIR chiusi){
    AIR = 1;
    setState(HVON);
    //}else AIR=0

  }
}

/*
 * stato 1, alta tensione attiva
 */
void HVON() {

  RTD = 0;
  AIR = 1;

  /*transizione verso stato DRIVE
    Il segnale dello SC è maggiore di 1,28 V
    suono RTD 2 secondi
  */
  if (analogRead(SC) > SCthr && get_brake_percentage() > RTDBK && digitalRead(RTDB) == LOW) { //brake >800

  //buzzer che suona
    for (int count=0; count<4; count++){
    delay(100);
    digitalWrite(BUZZ, HIGH);
    delay(270);
    digitalWrite(BUZZ, LOW);
    }
    
    //if(feedback RTD){
    RTD = 1;
    setState(DRIVE);
    //}else RTD=0
  }
  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  setState(STAND);
}



void DRIVE() {
  RTD = 1;
  AIR = 1;

  /*
    il while permette un ciclo più veloce evitando di
    ripetere la chiamata alla funzione di stato ad ogni richiesta di coppia
    appena scattano le plausibilità si esce dal while di guida plausibile
  */
  while (analogRead(SC) > SCthr && RTD && get_apps_plausibility() && get_brake_plausibility()) {

    /*drive!!*/
    if (get_apps_plausibility() && get_brake_plausibility()) {  
//      //mappa spezzata su richiesta coppia
//      if (th1 <= dinamica1 + th1Low)
//        analogWrite(DAC1, map(th1, th1Low, dinamica2 + th1Low, 0, 2047));
//      else
//        analogWrite(DAC1, map(th1, dinamica2 + th1Low, th1Up, 2048, 4095));

      if (bk<3500){  //3500=2.82V IN
        analogWrite(DAC1, map(th1, th1Low, th1Up, 0, 4095)); // regen
        analogWrite(DAC0, 0);
      }
      else{
        analogWrite(DAC1, 0);
        analogWrite(DAC0, 1613);  //1296=2.6V OUT
      }

      Serial.print("TH1: "); Serial.print(get_tps1_percentage()); Serial.print("   BREAK: "); Serial.println(get_brake_percentage());
    }
  }

  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  setState(STAND);

  /*scatto plausibilità TPS o scollegamento BRAKE*/
  if (!get_apps_plausibility() || bk < 500) {
    analogWrite(DAC1, map(th1Low, th1Low, th1Up, 0, 4095)); //TORQUE
    analogWrite(DAC0, 0);
    setState(NOTDRIVE);
  }


  /* scatto plausibilità pressione congiunta ACCELERATORE e FRENO
     ciclo di rientro pedale < 5% della corsa
  */
  if (!get_brake_plausibility()) {
    while (get_tps1_percentage() > RunTH5) {
      th1=analogRead(TPS1);
      th2=analogRead(TPS2);
      bk=analogRead(BRAKEIN);
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

    plaus2 = 1;
  }

  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  current_state = 0;

}

/* 
 * errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
 */
void NOTDRIVE() {
  if (analogRead(SC) < SCthr) current_state = 0;
  th1=analogRead(TPS1);
  th2=analogRead(TPS2) + 500; // + offset 
  bk=analogRead(BRAKEIN);
  /*
    th1-th2 <= Upper bound && th1-th2 >= Lower bound && bk < 10%
   **/
  if (th2 - th1 < Upper && th2 - th1 > Lower && bk < RunBK) {
    plaus1 = 1;
    current_state = 2;
    Serial.println("stato corrente 2");
  }

}

/* Tabella dei possibili stati:
  la tabella punta alle funzioni in elenco (stati)
*/
void(*state_table[])(void) = {STAND, HVON, DRIVE, NOTDRIVE};

void loop() {
  state_table[current_state]();
    currMillis2 = millis();

  if (currMillis2 - prevMillis2 >= plaus1Millis2) {
    prevMillis2 = currMillis2;
    Serial.print("SC= "); Serial.print(analogRead(SC)); Serial.print("  ");
    Serial.print("Stato corrente: "); Serial.print(current_state); Serial.print("  TPS1: "); Serial.print(analogRead(TPS1));
    Serial.print("    TPS2: "); Serial.print(analogRead(TPS2)); Serial.print("    Brake IN: "); Serial.println(analogRead(BRAKEIN));
  }
}
