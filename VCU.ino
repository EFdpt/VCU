#include <due_can.h>
#include <DueTimer.h>

/**
  PINOUT ENCODER MOTORE

  MOTORE          -> INVERTER
  ROSSO   VCC     -> 28
  BLU     GND     -> 24
  BIANCO  CLK+    -> 9
  VERDE   DATA+   -> 11
  MARR    CLK-    -> 10
  GIALLO  DATA-   -> 12
*/

#define INVERTER_NODE_ID      (0x01)
#define VCU_NODE_ID           (0x11)
#define SYNC_PERIOD           (5000) // 5ms

#define CAN_FUNZ              Can0

#define INVERTER_TORQUE_MAX   (32767)

volatile bool       inverter_online     = false;
volatile bool       inverter_configured = false;

volatile uint16_t   torque_actual_val   = 0;

uint32_t vendorID;

void send_sync() {
  CAN_FRAME sync;

  sync.id = 0x00000080;
  sync.length = 1;
  sync.extended = false;
  sync.data.byte[0] = 0x00;
  CAN_FUNZ.sendFrame(sync);
}

void CAN_GENERAL_CB(CAN_FRAME* frame) {

  if (frame -> id == (0x700 + INVERTER_NODE_ID)) {

    if (inverter_online)
      return;

    Serial.println("Received boot-up message from inverter");

    //Check inverter vendor ID (SDO client request)
    CAN_FRAME output;

    output.id = 0x00000600 + (uint8_t) VCU_NODE_ID ;
    output.length = 8;
    output.data.high = 0x40181001;
    output.data.low = 0x00000000;
    output.extended = 0;
    CAN_FUNZ.sendFrame(output);
    Serial.println("VendorID SDO client request sent");
    
    inverter_online = true;

  } else if (frame -> id == 0x580 + INVERTER_NODE_ID) {

    if (inverter_configured)
        return;

    vendorID = frame -> data.low;
    Serial.print("SDO server response with vendorID: ");
    Serial.println(vendorID, HEX);

    CAN_FRAME output;

    //pacchetto NMT comando 0x01=start remote node
    output.id = 0x00000000 ;
    output.length = 2 ;
    output.data.byte[0] = 0x01;
    output.data.byte[1] = 0x00;
    output.extended = 0;
    CAN_FUNZ.sendFrame(output);
    Serial.println("'Operational' NMT request sent");
    
    //pacchetto "shutdown" transizione 2 pagina 12 Firmware manual
    output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
    output.length = 8;
    output.data.high = 0x06000000;
    output.data.low = 0x00000000;
    output.extended = 0;
    CAN_FUNZ.sendFrame(output);

    //pacchetto "switch on" transizione 3 pagina 12 Firmware manual
    output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
    output.length = 8;
    output.data.high = 0x07000000;
    output.data.low = 0x00000000;
    output.extended = 0;
    CAN_FUNZ.sendFrame(output);

    output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
    output.length = 8;
    output.data.high = 0x0F000000;
    output.data.low = 0x00000000;
    output.extended = 0;
    CAN_FUNZ.sendFrame(output);

    inverter_configured = true;

    // send periodic SYNC
    DueTimer::getAvailable().attachInterrupt(send_sync).start(SYNC_PERIOD);
  } else if (frame -> id == 0x180 + INVERTER_NODE_ID) { // inverter torque actual val (Transmit PDO1)
    torque_actual_val = frame -> data.s3;
  }
}

void torque_request(uint16_t torque) {
  CAN_FRAME output;

  output.id = 0x00000200 + (uint8_t) VCU_NODE_ID;
  output.length = 8;
  output.data.high = 0x0F000000;
  output.data.low = 0x00000000 | map(torque, 0, 4095, 0, INVERTER_TORQUE_MAX);
  output.extended = 0;
  CAN_FUNZ.sendFrame(output);
}

void CAN_GENERAL_CONSUMER_CB(CAN_FRAME* frame) {
   Serial.print(" ID: 0x");
   Serial.print(frame->id, HEX);
   Serial.print(" Len: ");
   Serial.print(frame->length);
   Serial.print(" Data: 0x");
   for (int count = 0; count < frame->length; count++) {
       Serial.print(frame->data.bytes[count], HEX);
       Serial.print(" ");
   }
   Serial.print("\r\n");
}

/* attendere il pacchetto di boot: ID:0x700 + NODE-ID
 * scrivere il pacchetto chek vendorID = ID:0x611 | DATA: 0x40 1810 01 00000000
 * attendere il pacchetto con il vendorID= ID:0x590 | DATA: 0x43 1810 01 19030000
 * scrivere il pacchetto NMT= ID: 0x000 | DATA: 0x0100
*/
void CanBegin(){

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Serial started");
 
  while (!CAN_FUNZ.begin(CAN_BPS_500K)){
      Serial.println("CAN baudrate initialization ERROR");
  }
  
  uint32_t i = 0;

  for (; i < 8; i++) {
    CAN_FUNZ.setRXFilter(i, 0, 0, false);
    CAN_FUNZ.setCallback(i, CAN_GENERAL_CB);
  }

  Serial.println("CAN mailboxes configured");

  while (!inverter_online || !inverter_configured) {}

  Serial.println("Inverter successfully configured");
}

void setup() {
  analogReadResolution(12);
  CanBegin();
  torque_request(0);
}

volatile uint16_t apps_value;

void loop() {
    apps_value = (apps_value + analogRead(A0)) / 2;
    torque_request(apps_value);
    Serial.print("Torque actual value = ");
    Serial.println(torque_actual_val);
}
