/* questo codice sviluppato da Valerio Dodet è dedicato al funzionamento di un arduino DUE
   che svolga la funzione di unità di controllo della vettura FastCharge per il team
   Fast Charge in Formula SAE

    README: https://goo.gl/wGvLdF
    SITE: https://www.facebook.com/SapienzaFastCharge

   stati
   STAND:  stato 0, accensione della vettura, si ritorna qui ogni volta che casca l'SC

   HVON: stato 1, alta tensione attiva
      si accede solo da STAND tramite AIRbutton e SC>3V

   DRIVE:stato 2, lo stato di guida sicura, accedibile tramite procedura RTD ma anche con lo
      scatto delle plausibilità tramite procedura di rientro

   NOTDRIVE: stato 3, errore con la pedaliera, i sensori dei pedali sono scollegati o fuori
          range. Disabilitate richiesta di coppia e richiesta di frenata, si entra solo
          tramite scatto delle plausibilità o assenza del freno.

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
int th1Low = 1960;//2540
int th1Up = 2583;//3150
int th2Low = 2230;  //3100
int th2Up = 2930; //3400
int bkLow = 730;
int bkUp = 980;
int Upper = 1500;
int Lower = 0;
int SCthr = 600;


int lunghezza = 64; //lunghezza array di acquisizione
uint8_t lun = 6; //2^lun = lunghezza
int i = 0;
int j=0;
double irr; //numero per la funzione HASH

int RunTH = 2110; //25% della corsa del pedale
int RunTH5 = 1958;
int RunBK = 800; //10% della corsa del freno
int RTDBK = 800; //pressione freno per RTD

/*
   inzializzazione array acquisizioni:
   prima di 64 acquisizioni la dinamica sarà molto smorzata a causa di tutti questi zeri
*/
int ArrTh1[64]={0};
int ArrTh2[64]={0};
int ArrBk[64]={0};
int posArrTh1, posArrTh2, posArrBk;
int out[64]={0};

/*struttura dati per la lista di trabocco dell'hash table per il bucket sort
 **/
typedef struct lista {
  int chiave;
  struct lista *next;
}*lista_ptr;

//hash table
lista_ptr Htable[64] = {0};



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
  posArrTh1 = posArrTh2 = posArrBk = 0;

  for(i=0;i<lunghezza;i++){
    ArrTh1[i]=0;
    ArrTh2[i]=0;
    ArrBk[i]=0;
  }

  //azzeramento TORQUE OUT and BRAKE OUT
  analogWrite(DAC0, 0); //brake
  analogWrite(DAC1, map(th1Low + 20, th1Low, th1Up, 0, 4095)); //torque

  //inizializzazione del numero per la funzione hash
  irr = (sqrt(5) - 1) * 0.5;
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
  if ( digitalRead(AIRB) == LOW && analogRead(SC) > SCthr) {
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

  //acquisizione mediata freno per RTD
  for (i = 0; i < lunghezza; i++) {
    bk += analogRead(BRAKEIN);
  }
  bk >>= lun;
  Serial.print("bk ciclato: "); Serial.println(bk);

  /*transizione verso stato DRIVE
    Il segnale dello SC è maggiore di 1,28 V
    suono RTD 2 secondi
  */
  if (analogRead(SC) > SCthr && bk > RTDBK && digitalRead(RTDB) == LOW) { //brake >800

    digitalWrite(BUZZ, HIGH);
    delay(300);
    digitalWrite(BUZZ, LOW);
    delay(200);
    digitalWrite(BUZZ, HIGH);
    delay(300);
    digitalWrite(BUZZ, LOW);
    delay(200);
    digitalWrite(BUZZ, HIGH);
    delay(300);
    digitalWrite(BUZZ, LOW);
    delay(200);
    digitalWrite(BUZZ, HIGH);
    delay(300);
    digitalWrite(BUZZ, LOW);
    delay(200);
    digitalWrite(BUZZ, HIGH);
    delay(300);
    digitalWrite(BUZZ, LOW);
    delay(200);

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

    acquisizioneTPS1();
    acquisizioneTPS2();
    acquisizioneBSE();
    th1 = media(ArrTh1);
    th2 = media(ArrTh2);
    bk = media(ArrBk);

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

      Serial.print("TH1 media: "); Serial.print(th1); Serial.print("   BREAK media: "); Serial.println(bk);
    }

  }// end while

  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  current_state = 0;

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
    while (media(ArrTh1) > RunTH5) {
      acquisizioneTPS1();
      acquisizioneBSE();
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
   errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
*/
void NOTDRIVE() {
  if (analogRead(SC) < SCthr) current_state = 0;
  acquisizioneTPS1();
  acquisizioneTPS2();
  acquisizioneBSE();

  th1 = media(ArrTh1);
  th2 = media(ArrTh2);
  bk = media(ArrBk);
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
    Serial.print("SC= "); Serial.print(analogRead(SC)); Serial.print("  ");
    Serial.print("Stato corrente: "); Serial.print(current_state); Serial.print("  TPS1: "); Serial.print(analogRead(TPS1));
    Serial.print("    TPS2: "); Serial.print(analogRead(TPS2)); Serial.print("    Brake IN: "); Serial.println(analogRead(BRAKEIN));
  }
}


















