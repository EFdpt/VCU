/*
  stati:
  STAND: lo stato di accensione, si ritorna qui ogni volta che casca l'SC
  HVON: si accede solo da STAND tramite
        AIRbutton e SC>3V
  DRIVE: lo stato di guida sicura, accedibile
        tramite procedura RTD ma anche con lo
        scatto delle plausibilità tramite procedura di rientro
  NOTDRIVE: disabilitate richiesta di coppia e richiesta di frenata,
          si entra solo tramite scatto delle plausibilità o
          assenza del freno.

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

/* valori dei pedali in milliVolt
   analogReadResolution(12) ==> 4096=3300mV

*/
int th1Low = 1920;//2540
int th1Up = 2680;//3150
int th2Low = 2230;  //3100
int th2Up = 2930; //3400
int bkLow = 730;
int bkUp = 980;
int Upper = 500;
int Lower = 100;

int RunTH = 2110; //25% della corsa del pedale
int RunTH5 = 1958;
int RunBK = 770; //10% della corsa del freno


void setup() {
  // put your setup code here, to run once:
  pinMode(FAN, OUTPUT);
  pinMode(AIRcc, OUTPUT);
  pinMode(AIRGnd, OUTPUT);
  pinMode(PRE, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  //pinMode(AIRB, INPUT);
  // pinMode(RTDB, INPUT);
  pinMode(BRAKEIN, INPUT);
  pinMode(AIRB, INPUT_PULLUP);
  pinMode(RTDB, INPUT_PULLUP);
  Serial.begin(9600);
  analogWriteResolution(12);
  analogReadResolution(12);
}


/*
  stato 0, accensione della vettura
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
  if ( digitalRead(AIRB) == LOW && analogRead(SC) > 200) {
    digitalWrite(PRE, HIGH);
    digitalWrite(AIRGnd, HIGH);
    delay(3000);
    digitalWrite(PRE, LOW);
    digitalWrite(AIRcc, HIGH);
    //if(ricevuto pacchetto AIR chiusi){
    AIR = 1;
    current_state = 1;
    //}else AIR=0

  }
}

/*
   stato 1, alta tensione attiva
*/
void HVON() {

  RTD = 0;
  AIR = 1;
  plaus2 = 0;
  plaus1 = 0;

  /*transizione verso stato DRIVE
    Il segnale dello SC è maggiore di 1,28 V
    suono RTD 2 secondi
  */
  if (analogRead(SC) > 200 && analogRead(BRAKEIN) > RunBK && digitalRead(RTDB) == LOW) { //brake >770

    digitalWrite(BUZZ, HIGH);
    delay(2000);
    digitalWrite(BUZZ, LOW);
    //if(feedback RTD){
    RTD = 1;
    current_state = 2;
    //}else RTD=0
  }
  /*ritorno allo stato STAND*/
  if (analogRead(SC) < 200)  current_state = 0;
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
  while (analogRead(SC) > 200 && RTD && plaus1 && plaus2) {
    //    int [] arr = new arr[10];

    th1 = analogRead(TPS1);
    //    for(int i=0, i<10,i++){
    //      arr[i]=analogRead(TPS1);
    //    }
    //    int med=med(arr);

    th2 = analogRead(TPS2);

    bk = analogRead(BRAKEIN);


    //check plausibilità throttle
    currMillis = millis();

    if (currMillis - prevMillis >= plaus1Millis) {
      prevMillis = currMillis;

      /*
        th1-th2 <= Upper bound && th1-th2 >= Lower bound
       **/
      if (th2 - th1 > Upper || th2 - th1 < Lower) plaus1 = 0;
    }

    /*
      APPS >25% && brake attuato scatta seconda plausibilità
    */
    if (th1 > RunTH && bk > RunBK) plaus2 = 0;

    /*drive!!*/
    if (plaus1 && plaus2) {

      analogWrite(DAC1, map(th1, th1Low, th1Up, 0, 4095));
      analogWrite(DAC0, 0);
    }

  }// end while

  /*ritorno allo stato STAND*/
  if (analogRead(SC) < 200)  current_state = 0;

  /*scatto plausibilità TPS o scollegamento BRAKE*/
  if (!plaus1 || bk < 500) {
    analogWrite(DAC1, 0);
    analogWrite(DAC0, 0);
    current_state = 3;
  }


  /* scatto plausibilità pressione congiunta ACCELERATORE e FRENO
     ciclo di rientro pedale < 5% della corsa

  */
  if (!plaus2) {
    while (analogRead(TPS1) > RunTH5) {

Serial.println("!plaus2");
      //BYPASS BSPD
      if (analogRead(BRAKEIN) > 870) {
        digitalWrite(AIRGnd, LOW);
        digitalWrite(AIRcc, LOW);
        BSPD = 0;
        current_state = 0;
      }
      //END BYPASS BSPD
    }

    plaus2 = 1;
  }

   /*ritorno allo stato STAND*/
  if (analogRead(SC) < 200)  current_state = 0;

}

/*
   errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
*/
void NOTDRIVE() {
  if (analogRead(SC) < 200) current_state = 0;

  th1 = analogRead(TPS1);
  th2 = analogRead(TPS2);
  bk = analogRead(BRAKEIN);

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
  //delay(5);
  currMillis2 = millis();

  if (currMillis2 - prevMillis2 >= plaus1Millis2) {
    prevMillis2 = currMillis2;
    Serial.print("Stato corrente: "); Serial.print(current_state); Serial.print("  TPS1: "); Serial.print(analogRead(TPS1)); Serial.print("    TPS2: "); Serial.print(analogRead(TPS2)); Serial.print("    Brake IN: "); Serial.println(analogRead(BRAKEIN));
    Serial.print("   SC= "); Serial.print(analogRead(SC));
    Serial.print("AIRMENO:  ");Serial.println(AIRGnd);

  }
}

















