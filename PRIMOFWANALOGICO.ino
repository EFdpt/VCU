
const int AIRcc = 48;
const int AIRGnd = 49;
const int PRE = 24;
const int BUZZ = 47;
const int BrakeIN = A2;
const int AIRB = 14;
const int RTDB = 15;

int thr1 = A0;
int thr2 = A1;
int BRAKEIN = A2;
int SC = A3;
int AIR = 0;
int RTD = 0;
int th1 = 0;
int th2 = 0;
int bk = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(AIRcc, OUTPUT);
  pinMode(AIRGnd, OUTPUT);
  pinMode(PRE, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(AIRB, INPUT);
  pinMode(RTDB, INPUT);
  pinMode(BrakeIN, INPUT);
  pinMode(AIRB, INPUT_PULLUP);
  pinMode(RTDB, INPUT_PULLUP);
  Serial.begin(9600);
  analogWriteResolution(12);
}

void loop() {

  while (!AIR) {

    if ( digitalRead(AIRB) == LOW && analogRead(SC) > 200) {
      digitalWrite(PRE, HIGH);
      digitalWrite(AIRGnd, HIGH);
      delay(3000);
      digitalWrite(PRE, LOW);
      digitalWrite(AIRcc, HIGH);
      AIR = 1;
    }
  }
  RTD = 0;
  while (!RTD) {

    if (analogRead(SC) > 200 && analogRead(BrakeIN) > 300 && digitalRead(RTDB) == LOW ) { // Il segnale dello SC è maggiore di 1,28 V


      digitalWrite(BUZZ, HIGH);
      delay(3500);
      digitalWrite(BUZZ, LOW);
      RTD = 1;
    }
  }

  while (analogRead(SC) > 200 && RTD) {
    th1 = analogRead(thr1);
    th2 = analogRead(thr2);
    bk = analogRead(BRAKEIN);
    if (th1 - th2 <= 50 && bk < 200 ) {

      analogWrite(DAC1, map(th1, 0, 1023, 0, 4095));
    }
    else {
      analogWrite(DAC1, 0);
    }

  }
}















