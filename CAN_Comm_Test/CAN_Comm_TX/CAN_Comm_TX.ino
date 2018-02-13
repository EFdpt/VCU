#include "common.h"
#include <due_can.h>

#define CAN_PORT              Can0
#define CAN_TX_ID             0x11
#define CAN_FRAME_PRIORITY    0

#define POT_PIN               A0

#define TX_TIMING             1
#define PERIOD                10000 // 10ms

#if TX_TIMING
#include <DueTimer.h>
#endif

uint16_t read_data;

static void sendData(uint16_t data) {
  CAN_FRAME outgoing;
  outgoing.id = CAN_TX_ID;
  outgoing.extended = false;
  outgoing.priority = CAN_FRAME_PRIORITY;
  outgoing.length = 2;
  outgoing.data.s0 = data;
  if (!CAN_PORT.sendFrame(outgoing)) {
    #if VERBOSE_LEVEL > 1
      Serial.println("[CAN ERROR] Failed to send CAN packet\r");
    #endif
    return;
  }
  
 #if VERBOSE_LEVEL > 1
  printFrame(&outgoing);
 #endif
}

void TX_TIMER_IRQ() {
  sendData(read_data);
}

void setup() {
  analogReadResolution(12);
  CAN_PORT.begin(CAN_BAUDRATE);

 #if TX_TIMING
  Timer3.attachInterrupt(TX_TIMER_IRQ);
  Timer3.start(PERIOD);
 #endif

 #if VERBOSE_LEVEL > 1
  Serial.begin(9600);
 #endif
}

void loop() {
  #if TX_TIMING
  noInterrupts();
  #endif
  read_data = analogRead(POT_PIN);
  #if TX_TIMING
  interrupts();
  #endif
  
  #if !TX_TIMING
  sendData(read_data);
  #endif
}
