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

#include <due_can.h>
#include <DueTimer.h>

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

int SC = A3;
int AIR = 0;
int RTD = 0;
int th1 = 0;
int th2 = 0;
int bk = 0;
int plaus2 = 0;
int plaus1 = 0;
int BSPD = 1;

int SCthr=600;

int RunTH = 25; //25% della corsa del pedale
int RunTH5 = 5; // 5% della corsa del pedale
int RunBK = 10; //10% della corsa del freno
int RTDBK = 10; //pressione freno per RTD

#define VCU_TIME_PERIOD		(20000) // 20ms

#define VCU_TIME_SLOT		(0x15)
#define PEDALI_ID			(0x10)
#define _INVERTER 0

CAN_FRAME VCU_timing;

#define CAN_FUNZ    Can1
#define CAN_SERV    Can0

void CAN_RX_Callback(CAN_FRAME* packet) {
  noInterrupts();
	if (packet -> id == PEDALI_ID) {
		th1 = packet -> data.bytes[0];
		th2 = packet -> data.bytes[1];
		bk = packet -> data.bytes[2];
		//plaus1 = 0xF0 & packet.data.bytes[3];
		//plaus2 = 0x0F & packet.data.bytes[3];
	}
  interrupts();
}

// da chiamare in Timer IRQ handler
void CAN_pack() {
  noInterrupts();
	VCU_timing.data.bytes[0] = current_state;
  interrupts();
}

void Timer_IRQ() {
	CAN_pack();
	CAN_SERV.sendFrame(VCU_timing);
}

/* attendere il pacchetto di boot: ID:x710
 * scrivere il pacchetto chek vendorID= ID:x611 | DATA: x4018100100000000
 * attendere il pacchetto con il vendorID= ID:x590 | DATA: x4318100119030000
 * scrivere il pacchetto NMT= ID: x000 | DATA: x0100
*/
void CanBegin(){
	
	VCU_timing.id = (uint32_t) VCU_TIME_SLOT;
	VCU_timing.length = 1;
	VCU_timing.data.bytes[0] = current_state;
	VCU_timing.extended = 0;
	
	if(!CAN_SERV.begin(1000000,51)) { // 1000000 baudrate = 1MHz
        Serial.println("CAN servizi initialization (sync) ERROR");
        //morteeeeee
        while(1);
    }
    
    CAN_SERV.setGeneralCallback(CAN_RX_Callback);
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

  CanBegin();
  
  Timer3.attachInterrupt(Timer_IRQ);
  Timer3.start(VCU_TIME_PERIOD);

  //azzeramento TORQUE OUT and BRAKE OUT
  analogWrite(DAC0, 0); //brake
  //analogWrite(DAC1, map(th1Low, th1Low, th1Up, 0, 4095)); //torque
  analogWrite(DAC1, 0); //torque
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
    digitalWrite(PRE, LOW);
    digitalWrite(AIRcc, HIGH);
    AIR = 1;
    current_state = 1;
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

  /*transizione verso stato DRIVE
    Il segnale dello SC è maggiore di 1,28 V
    suono RTD 2 secondi
  */
  if (analogRead(SC) > SCthr && digitalRead(RTDB) == LOW && bk > RTDBK) { // brake > 10%

    //buzzer che suona
    for (int count=0; count<4; count++){
		delay(100);
		digitalWrite(BUZZ, HIGH);
		delay(270);
		digitalWrite(BUZZ, LOW);
    }

    RTD = 1;
    current_state = 2;
  }
  
  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  current_state = 0;
}

void DRIVE() {
  RTD = 1;
  AIR = 1;
  plaus2 = 1;
  plaus1 = 1;

  while (analogRead(SC) > SCthr && RTD) {
    /*drive!!*/
    analogWrite(DAC1, map(th1, 0, 100, 0, 4095));
    analogWrite(DAC0, 0);
    Serial.print("TH1: "); Serial.print(th1); Serial.print("   BREAK: "); Serial.println(bk);
  }// end while

  /*ritorno allo stato STAND*/
  if (analogRead(SC) < SCthr)  current_state = 0;
}

/* 
 * errore con la pedaliera, i sensori dei pedali sono scollegati o fuori range
 */
void NOTDRIVE() {
	if (analogRead(SC) < SCthr) current_state = 0;
	uint8_t distance = th1 > th2 ? th1 - th2 : th2 - th1;
	if (distance < 10) {
		plaus1 = 1;
		current_state = 2;
		Serial.println("stato corrente DRIVE");
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
    Serial.print("Stato corrente: "); Serial.print(current_state); Serial.print("  TPS1: "); Serial.print(th1);
    Serial.print("    TPS2: "); Serial.print(th2); Serial.print("    Brake IN: "); Serial.println(bk);
  }
}
