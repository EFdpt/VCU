/* questo codice sviluppato da Valerio Dodet è dedicato alla calibrazione
    dei valori dei sensori della pedaliera.

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

unsigned long prevMillis = 0;
unsigned long currMillis = 0;
const long plaus1Millis = 500;
unsigned long prevMillis2 = 0;
unsigned long currMillis2 = 0;
const long plaus1Millis2 = 20000;


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



int readTh1, readTh2, readBk;
int minTh1, maxTh1; //variabili globali per la funzione HASH
int minTh2, maxTh2;
int minBk, maxBk;
uint8_t calibrato = 0;


void APPS() {
  Serial.println();
  Serial.println();
  Serial.println();
  currMillis2 = prevMillis2 = millis();
  while (!calibrato) {
    readTh1 = analogRead(TPS1);
    readTh2 = analogRead(TPS2);
    if (minTh1 > readTh1)    minTh1 = readTh1;
    if (minTh2 > readTh2)    minTh2 = readTh2;
    if (maxTh1 < readTh1)    maxTh1 = readTh1;
    if (maxTh2 < readTh2)    maxTh2 = readTh2;

    currMillis = millis();
    if (currMillis - prevMillis >= plaus1Millis) {
      prevMillis = currMillis;
      Serial.println();
      Serial.println();
      Serial.print("TPS1 =["); Serial.print(minTh1); Serial.print(" - "); Serial.print(maxTh1); Serial.print("]");
      Serial.print("TPS2 =["); Serial.print(minTh2); Serial.print(" - "); Serial.print(maxTh2); Serial.println("]");
    }
    currMillis2 = millis();
    if (currMillis2 - prevMillis2 >= plaus1Millis2) {
      calibrato = 1;
      digitalWrite(BUZZ, HIGH);
      delay(500);
      digitalWrite(BUZZ, LOW);
    }
  }
  calibrato = 0;
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("FINE CALIBRAZIONE ACCELERATORE");
}


void BRAKE() {
  Serial.println();
  Serial.println();
  Serial.println();
  currMillis2 = prevMillis2 = millis();
  while (!calibrato) {
    readBk = analogRead(BRAKEIN);
    if (minBk > readBk)    minBk = readBk;
    if (maxBk < readBk)    maxBk = readBk;

    currMillis = millis();
    if (currMillis - prevMillis >= plaus1Millis) {
      prevMillis = currMillis;
      Serial.println();
      Serial.println();
      Serial.print("BRAKE =["); Serial.print(minBk); Serial.print(" - "); Serial.print(maxBk); Serial.println("]");
    }
    currMillis = millis();
    if (currMillis2 - prevMillis2 >= plaus1Millis2) {
      calibrato = 1;
      digitalWrite(BUZZ, HIGH);
      delay(500);
      digitalWrite(BUZZ, LOW);
    }
  }
  calibrato = 0;
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("FINE CALIBRAZIONE FRENO");
}

void NUMERI() {

  Serial.println();
  Serial.print("TPS1 =["); Serial.print(minTh1); Serial.print(" - "); Serial.print(maxTh1); Serial.print("]");
  Serial.print("TPS2 =["); Serial.print(minTh2); Serial.print(" - "); Serial.print(maxTh2); Serial.println("]");
  Serial.println();
  Serial.print("BRAKE =["); Serial.print(minBk); Serial.print(" - "); Serial.print(maxBk); Serial.println("]");
  Serial.println();
  Serial.print("TPS1 5%="); Serial.println((((double)(maxTh1 - minTh1)) / 20) + minTh1);
  Serial.println();
  Serial.print("TPS1 25%="); Serial.println((((double)(maxTh1 - minTh1)) / 4) + minTh1);
  Serial.println();
  Serial.print("TPS2 5%="); Serial.println((((double)(maxTh2 - minTh2)) / 20) + minTh2);
  Serial.println();
  Serial.print("TPS2 25%="); Serial.println((((double)(maxTh2 - minTh2)) / 4) + minTh2);
  Serial.println();
  Serial.print("BRAKE 10%="); Serial.println((((double)(maxBk - minBk)) / 10) + minBk);
  Serial.println();
  Serial.print("BRAKE 25%="); Serial.println((((double)(maxBk - minBk)) / 4) + minBk);
  Serial.println();

}

void setup() {
  // put your setup code here, to run once:
  pinMode(FAN, OUTPUT);
  pinMode(AIRcc, OUTPUT);
  pinMode(AIRGnd, OUTPUT);
  pinMode(PRE, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(AIRB, INPUT_PULLUP);
  pinMode(RTDB, INPUT_PULLUP);
  Serial.begin(9600);
  analogWriteResolution(12);
  analogReadResolution(12);

  minTh1 = minTh2 = minBk = 4095;
  maxTh1 = maxTh2 = maxBk = 0;

  //azzeramento TORQUE OUT and BRAKE OUT
  analogWrite(DAC0, 0); //brake
  analogWrite(DAC1, 0); //torque
}

void loop() {
  Serial.println("inizio calibrazione sensori piedali acceleratore: 20s");

  digitalWrite(BUZZ, HIGH);
  delay(300);
  digitalWrite(BUZZ, LOW);
  delay(200);
  digitalWrite(BUZZ, HIGH);
  delay(300);
  digitalWrite(BUZZ, LOW);
  delay(200);
  digitalWrite(BUZZ, HIGH);
  delay(500);
  digitalWrite(BUZZ, LOW);

  APPS();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("inizio calibrazione sensore pedale freno: 20s");

  digitalWrite(BUZZ, HIGH);
  delay(300);
  digitalWrite(BUZZ, LOW);
  delay(200);
  digitalWrite(BUZZ, HIGH);
  delay(300);
  digitalWrite(BUZZ, LOW);
  delay(200);
  digitalWrite(BUZZ, HIGH);
  delay(500);
  digitalWrite(BUZZ, LOW);

  BRAKE();
  Serial.println();
  Serial.println();
  Serial.println("FINE calibrazione sensori pedaliera freno:");
  NUMERI();
  Serial.println();
  Serial.println();
  Serial.println("===============   Grazie dell'attenzione   ===============");
  Serial.println("while(1);");
  while (1);

}


















