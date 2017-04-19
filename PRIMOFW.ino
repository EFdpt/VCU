#include "due_can.h"
const int AIRcc=5;
const int AIRGnd=6;
const int PRE=7;
const int BUZZ=8;
const int BrakeIN=9;
int thr1=A0;
int thr2=A1;
int Brake1=A2;
int SC= A3;
int AIR=0;
int RTD=0;
int th1=0;
int th2=0;
int bk1=0;
int bk2=0;
void setup() {
  // put your setup code here, to run once:
  pinMode(AIRcc,OUTPUT);
  pinMode(AIRGnd,OUTPUT);
  pinMode(PRE,OUTPUT);
  pinMode(BUZZ,OUTPUT);
  pinMode(BrakeIN,INPUT);
Serial.begin(115200);
Can0.begin(1000000,50);//inizializza la porta con 1Mb/s e porta alto il pin 51 quando la porta è abilitata
Can1.begin(1000000,51);//inizializza la porta con 1Mb/s e porta alto il pin 51 quando la porta è abilitata
}

void loop() {
  CAN_FRAME BUTTONs;
  CAN_FRAME PEDALIERA;
  CAN_FRAME TORQUE;
  CAN_FRAME INVERTER;
  
  while(!AIR){
  Can1.watchFor(0xA0); //lascia passare solo i pacchetti con identificativo A0
  if(Can1.available()>0){
    Can1.read(BUTTONs);
   if( BUTTONs.data.byte[0]==1){ //il primo bit del pacchetto ricevuto è uguale a 1
    digitalWrite(PRE,HIGH);
    digitalWrite(AIRGnd,HIGH);
    delay(3000);
    digitalWrite(PRE,LOW);
    digitalWrite(AIRcc,HIGH);
    AIR=1;
   }
   }
  }
  while(!RTD){
    if(analogRead(SC)>400 && digitalRead(BrakeIN)){
      if(Can1.available()>0){
        Can1.read(BUTTONs);
        if(BUTTONs.data.byte[1]==1){
          digitalWrite(BUZZ,HIGH);
          delay(3500);
          digitalWrite(BUZZ,LOW);
          RTD=1;
        }
        }
      }
  }
  while(analogRead(SC)>400 && RTD){
    Can1.watchFor(0xA1);
    if(Can1.available()>0){
      Can1.read(PEDALIERA);
      if(PEDALIERA.id==0xA1){
      //i segnali th1,th2,bk1 e bk2 arriveranno già opportunamente mappati da 0 a 100
      th1=PEDALIERA.data.byte[0];
      th2=PEDALIERA.data.byte[1];
      bk1=PEDALIERA.data.byte[2];
      bk2=PEDALIERA.data.byte[3];
      
      }
    }
  }
    while((th1-th2)<10 && (bk1-bk2)<10 ){ //quanto la plausibiltà sugli acceleratori è verificata,richiedo coppia
      if(th1>25 && bk1>5){} //se freno con più del 25% dell'acceleratore è implausibilità quindi non richiedo coppia 
      else{
        INVERTER.id=0x01;
        INVERTER.extended = false;
        INVERTER.priority = 0;
        INVERTER.data.byte[0]=th1;
        INVERTER.data.byte[1]=th2;
        INVERTER.data.byte[2]=bk1;
        Can0.sendFrame(INVERTER);
      }
    }
  }
                 
        
        
              
  
  
    
   
