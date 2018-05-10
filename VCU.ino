#include <due_can.h>

#define INVERTER_NODE_ID      (0x01)
#define VCU_NODE_ID           (0x11)

#define CAN_FUNZ              Can0

volatile bool       inverter_online     = false;
volatile bool       inverter_configured = false;

uint32_t vendorID;

void CAN_GENERAL_CB(CAN_FRAME* frame) {

  if (frame -> id == (0x700 + INVERTER_NODE_ID)) {

    inverter_online = true;
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

  } else if (frame -> id == 0x580 + INVERTER_NODE_ID) {

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
  }
}

/* attendere il pacchetto di boot: ID:0x700 + NODE-ID
 * scrivere il pacchetto chek vendorID = ID:0x611 | DATA: 0x40 1810 01 00000000
 * attendere il pacchetto con il vendorID= ID:0x590 | DATA: 0x43 1810 01 19030000
 * scrivere il pacchetto NMT= ID: 0x000 | DATA: 0x0100
*/
void CanBegin(){
 
  while (!CAN_FUNZ.begin(CAN_BPS_1000K)){
      Serial.println("CAN baudrate initialization ERROR");
  }

  // boot-up message
  //CAN_FUNZ.setRXFilter(0, 0x00000700 + INVERTER_NODE_ID, 0x1FFFFFFF, false);
  //CAN_FUNZ.setCallback(0, INVERTER_BOOT_UP_CB);

  // inverter vendor ID
  //CAN_FUNZ.setRXFilter(1, 0x00000580 + INVERTER_NODE_ID, 0x1FFFFFFF, false);
  //CAN_FUNZ.setCallback(1, INVERTER_VENDOR_ID_CB);

  uint32_t i = 0;

  for (; i < 8; i++) {
    CAN_FUNZ.setRXFilter(i, 0, 0, false);
    CAN_FUNZ.setCallback(i, CAN_GENERAL_CB);
  }

  while (!inverter_online || !inverter_configured) {}
}

void setup() {
  CanBegin();
}

void loop() {
}
