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
uint8_t lunghezza=32;
uint8_t current_state = 0;

unsigned long prevMillis = 0;
unsigned long currMillis = 0;
const long plaus1Millis = 50;//tempo tra un check e l'altro della coerenza degli APPS (da regolamento è max 100ms)
unsigned long prevMillis2 = 0;
unsigned long currMillis2 = 0;
const long plaus1Millis2 = 100;


const int AIRcc = 48;
const int AIRGnd = 49;
const int PRE = 24;
const int BUZZ = 47;
const int AIRB = 14;
const int RTDB = 15;

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

/* valori dei pedali in milliVolt
   analogReadResolution(12) ==> 4096=3300mV

*/
int th1Low = 1920; 
int th1Up = 2680;
int th2Low = 2230;  
int th2Up = 2930; 
int bkLow = 730;
int bkUp = 980;
int Upper = 1500; //  reale per rispettare offset tra potenziometri , basta che sia > 350 - 400
int Lower = 0; // reale per rispettare offset tra potenziometri a 220

int RunTH = 2110; //25% della corsa throttle1
int RunTH5 = 1958; //5% della corsa throttle1
int RunBK = 770; //10% della corsa del freno

int ArrTh1[32];
int ArrTh2[32];
int ArrBk[32]; 
int posArrTh1, posArrTh2, posArrBk;


void setup() {
  // put your setup code here, to run once:
  pinMode(AIRcc, OUTPUT);
  pinMode(AIRGnd, OUTPUT);
  pinMode(PRE, OUTPUT);
  pinMode(BUZZ, OUTPUT);
//  pinMode(AIRB, INPUT);
//  pinMode(RTDB, INPUT);
  pinMode(BRAKEIN, INPUT);
  pinMode(AIRB, INPUT_PULLUP);
  pinMode(RTDB, INPUT_PULLUP);
  Serial.begin(9600);
  analogWriteResolution(12);
  analogReadResolution(12);
}


/*
 *ritorna la media di un array di 64 posizioni
 */
int media(int* ar){
  int out=0;
 
  for(int i=0;i<lunghezza;i++){
    out+=ar[i];
  }
  return out>>5; //32, shift 6 per 64 valori 
}

/*
 *acquisizione ADC circolare su array da 32 posizioni*/
void acquisizione(int* ar,int pinADC, int* posizione){
  if(*posizione<lunghezza)  
    ar[*posizione++] = analogRead(pinADC);
  else  
    ar[0]=analogRead(pinADC);
  }

/*
  stato 0, accensione della vettura
*/
void STAND() {
  //invio pacchetto VCU_OK su CAN
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
  if (analogRead(SC) > 200 && analogRead(BRAKEIN) > 300 && digitalRead(RTDB) == LOW) {

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
    acquisizione(ArrTh1, TPS1, &posArrTh1);
    acquisizione(ArrTh2, TPS2, &posArrTh2);
    acquisizione(ArrBk, BRAKEIN, &posArrBk);
  
    
    //check plausibilità throttle
    currMillis = millis();

    if (currMillis - prevMillis >= plaus1Millis) {
      prevMillis = currMillis;

      /*TODO trovare margine di plausibilità
        th2-th1 <= Upper bound && th2-th1 >= Lower bound
       **/
       th2=media(ArrTh2);
       th1=media(ArrTh1);
      if (th2 - th1> Upper || th2 - th1 < Lower) plaus1 = 0;
    }

    /*TODO
     * trovare soglie per plausibilità
      APPS >25% && brake attuato scatta seconda plausibilità
    */
    if (th1 > RunTH && bk > RunBK) plaus2 = 0;

    /*drive!!*/
    if (plaus1 && plaus2) {
      
      analogWrite(DAC1, map(media(ArrTh1), th1Low, th1Up, 0, 4095));
      analogWrite(DAC0, map(media(ArrBk), bkLow, bkUp, 0, 4095));
    }
 
  }// end while

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
    while (media(ArrTh1) > RunTH5){
      acquisizione(ArrTh1, TPS1, &posArrTh1);
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
  
    acquisizione(ArrTh1, TPS1, &posArrTh1);
    acquisizione(ArrTh2, TPS2, &posArrTh2);
    acquisizione(ArrBk, BRAKEIN, &posArrBk);
  /*
    th1-th2 <= Upper bound && th1-th2 >= Lower bound && bk < 10%
   **/
   th1=media(ArrTh1);
   th2=media(ArrTh2);
   bk=media(ArrBk);
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
    Serial.print("Stato corrente: "); Serial.print(current_state); 
    Serial.print("  TPS1: "); Serial.print(analogRead(TPS1)); 
    Serial.print("    TPS2: "); Serial.print(analogRead(TPS2)); 
    Serial.print("    Brake IN: "); Serial.println(analogRead(BRAKEIN));
    Serial.print("    TPS1Mediato: "); Serial.println(media(ArrTh1));
    Serial.print("    TPS2Mediato: "); Serial.println(media(ArrTh2));
    Serial.print("    BrakeMediato: "); Serial.println(media(ArrBk));
    Serial.print("   SC= "); Serial.print(analogRead(SC));

  }
}







