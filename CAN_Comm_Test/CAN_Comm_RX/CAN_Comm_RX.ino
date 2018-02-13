#include "common.h"
#include <due_can.h>

#define CAN_PORT              Can1
#define LED_PIN               5

#define POT_THRES             500
#define CAN_BLOCKING          1

#if CAN_BLOCKING
CAN_FRAME                     incoming;
#endif

static inline void consumer(CAN_FRAME* frame) {
  
  if (frame -> data.s0 > POT_THRES)
    digitalWrite(LED_PIN, HIGH);
  else
    digitalWrite(LED_PIN, LOW);

  #if VERBOSE_LEVEL > 0
  printFrame(frame);
  #endif
}

void CAN_RX_Callback(CAN_FRAME* frame) {
  consumer(frame);
}

void setup() {
  CAN_PORT.begin(CAN_BAUDRATE);
  pinMode(LED_PIN, OUTPUT);
  
 #if VERBOSE_LEVEL > 0
  Serial.begin(SERIAL_BAUDRATE);
 #endif

 #if !CAN_BLOCKING
 CAN_PORT.setGeneralCallback(CAN_RX_Callback);
 #endif
}

void loop() {
  #if CAN_BLOCKING
  if (CAN_PORT.available() > 0) {
    CAN_PORT.read(incoming);
    consumer(&incoming);
  }
  #endif
}
