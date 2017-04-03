#include <due_can.h>
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
  CAN_FRAME AIRBUTTON;
  CAN_FRAME RTDBUTTON;
  while(!AIR){
  Can1.watchFor(0xA0); //lascia passare solo i pacchetti con identificativo A0
  if(Can1.available()>0){
    Can1.read(AIRBUTTON);
   if( AIRBUTTON.data.byte[0]==1){ //il primo bit del pacchetto ricevuto è uguale a 1
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
    Can1.watchFor(0xA1);// lascia passare i pacchetti con identificativo A1
    if(analogRead(SC)>400 && digitalRead(BrakeIN)){
      if(Can1.available()>0){
        Can1.read(RTDBUTTON);
        if(RTDBUTTON.id==0xA1 && RTDBUTTON.data.byte[0]==1){
          digitalWrite(BUZZ,HIGH);
          delay(3500);
          digitalWrite(BUZZ,LOW);
          RTD=1;
        }
        }
      }
  }
}
  
    
   
