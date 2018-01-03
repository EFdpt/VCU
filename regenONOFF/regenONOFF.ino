/* questo codice sviluppato da Valerio Dodet è dedicato al funzionamento di un arduino DUE
 * che svolga la funzione di unità di controllo della vettura FastCharge per il team
 * Fast Charge in Formula SAE
 *  
 *  README: https://goo.gl/wGvLdF
 *  SITE: https://www.facebook.com/SapienzaFastCharge
 *    
 * stati
 * STAND:  stato 0, accensione della vettura, si ritorna qui ogni volta che casca l'SC
 *
 * HVON: stato 1, alta tensione attiva
 *    si accede solo da STAND tramite AIRbutton e SC>3V
 *
 * DRIVE:stato 2, lo stato di guida sicura, accedibile tramite procedura RTD ma anche con lo 
 *    scatto delle plausibilità tramite procedura di rientro
 *
 * NOTDRIVE: stato 3, errore con la pedaliera, i sensori dei pedali sono scollegati o fuori
 *        range. Disabilitate richiesta di coppia e richiesta di frenata, si entra solo
 *        tramite scatto delle plausibilità o assenza del freno.
 *
 */



uint8_t current_state = 0;

unsigned long prevMillis = 0;
unsigned long currMillis = 0;
const long plaus1Millis = 50;//tempo tra un check e l'altro della coerenza degli APPS (da regolamento è max 100ms)
unsigned long prevMillis2 = 0;
unsigned long currMillis2 = 0;
const long plaus1Millis2 = 100;


const int AIRcc = 48;
const int AIRGnd = 49;
const int PRE = 47;
const int BUZZ = 52;
const int AIRB = 14;
const int RTDB = 15;
const int FAN = 9;

int TPS1 = A0;
int TPS2 = A1;
int BRAKEIN = A2;
int SC = A3;
int AIR = 0;
int RTD = 0;
int th1 = 0;
int th2 = 0;
int bk = 0;
int plaus2 = 0;
int plaus1 = 0;
int BSPD = 1;


int th1Low = 2820;//2540
int th1Up = 4080;
int th2Low = 3500;  //3100
int th2Up = 4595; //3400
int bkLow = 1450;
int bkUp = 1780;
int Upper = 1400; // 900 + offset
int Lower = 500;
int SCthr=600;
int dinamica1 = 885;  //75% di th1Up-th1Low utilizzato per mappa pedale non lineare
int dinamica2 = 944; //80% della corsa del pedale

int lunghezza = 16; //lunghezza array di acquisizione
uint8_t lun = 4; //2^lun = lunghezza
int i=0;

int RunTH = 3195; //25% della corsa del pedale
int RunTH5 = 2959;
int RunBK = 1700; //10% della corsa del freno
int RTDBK = 1700; //pressione freno per RTD

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
  Serial.begin(9600);
  analogWriteResolution(12);
  analogReadResolution(12);


  //azzeramento TORQUE OUT and BRAKE OUT
  analogWrite(DAC0, 0); //brake
  analogWrite(DAC1, map(th1Low, th1Low, th1Up, 0, 4095)); //torque

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
  if ( digitalRead(AIRB) == LOW && analogRead(SC) > SCthr) {
    digitalWrite(PRE, HIGH);
    digitalWrite(AIRGnd, HIGH);
    delay(3000);
    digitalWrite(AIRcc, HIGH);
    digitalWrite(PRE, LOW);
    //if(ricevuto pacchetto AIR chiusi){
    AIR = 1;
    current_state = 1;
    //}else AIR=0

  }
}

/*
 * stato 1, alta tensione attiva
 */
void HVON() {

  RTD = 0;
  AIR = 1;
  plaus2 = 0;
  plaus1 = 0;

  //acquisizione mediata freno per RTD
  for (int i = 0; i < lunghezza; i++) {
    bk += analogRead(BRAKEIN);
  }
  bk >>= lun;
Serial.print("bk ciclato: "); Serial.println(bk);  

  /*transizione verso stato DRIVE
    Il segnale dello SC è maggiore di 1,28 V
    suono RTD 2 secondi
  */
  if (analogRead(SC) > SCthr && bk > RTDBK && digitalRead(RTDB) == LOW) { //brake >800

  //buzzer che suona
    for (int count=0; count<4; count++){
    delay(100);
    digitalWrite(BUZZ, HIGH);
    delay(270);
    digitalWrite(BUZZ, LOW);
    }
    
    //if(feedback RTD){
    RTD = 1;
    current_state = 2;
    //}else RTD=0
  }
  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  current_state = 0;
}



void DRIVE() {
  RTD = 1;
  AIR = 1;
  plaus2 = 1;
  plaus1 = 1;

  /*
    il while permette un ciclo più veloce evitando di
    ripetere la chiamata alla funzione di stato ad ogni richiesta di coppia

    appena scattano le plausibilità si esce dal while di guida plausibile
  */
  while (analogRead(SC) > SCthr && RTD && plaus1 && plaus2) {

    /*acquisizione diretta pedali*/
    th1=analogRead(TPS1);
    th2=analogRead(TPS2) + 500; // + offset per plaus1
    bk=analogRead(BRAKEIN);
    {
//    //check plausibilità throttle
//    currMillis = millis();
//
//    if (currMillis - prevMillis >= plaus1Millis) {
//      prevMillis = currMillis;
//
//      /*
//        th1-th2 <= Upper bound && th1-th2 >= Lower bound
//       **/
//      if (th2 - th1 > Upper || th2 - th1 < Lower) plaus1 = 0;
//    }
//
//    /*
//      APPS >25% && brake attuato scatta seconda plausibilità
//    */
//    if (th1 > RunTH && bk > RunBK) plaus2 = 0;
}
     
    /*drive!!*/
    if (plaus1 && plaus2) {
      if(th1<th1Low)  th1=th1Low;
      if(th1>th1Up) th1=th1Up-20;
  
//      //mappa spezzata su richiesta coppia
//      if (th1 <= dinamica1 + th1Low)
//        analogWrite(DAC1, map(th1, th1Low, dinamica2 + th1Low, 0, 2047));
//      else
//        analogWrite(DAC1, map(th1, dinamica2 + th1Low, th1Up, 2048, 4095));

      if (bk<3500){  //3500=2.82V IN
        analogWrite(DAC1, map(th1, th1Low, th1Up, 0, 4095));
        analogWrite(DAC0, 0);
      }
      else{
        analogWrite(DAC1, 0);
        analogWrite(DAC0, 1613);  //1296=2.6V OUT
      }

      Serial.print("TH1: "); Serial.print(th1); Serial.print("   BREAK: "); Serial.println(bk);
    }

  }// end while

  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  current_state = 0;

  /*scatto plausibilità TPS o scollegamento BRAKE*/
  if (!plaus1 || bk < 500) {
    analogWrite(DAC1, map(th1Low, th1Low, th1Up, 0, 4095)); //TORQUE
    analogWrite(DAC0, 0);
    current_state = 3;
  }


  /* scatto plausibilità pressione congiunta ACCELERATORE e FRENO
     ciclo di rientro pedale < 5% della corsa

  */
  if (!plaus2) {
    while (analogRead(TPS1) > RunTH5) {
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



