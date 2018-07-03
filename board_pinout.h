#ifndef _BOARD_PINOUT_H_
#define _BOARD_PINOUT_H_

#define CAN_FUNZIONALE              Can1 // DAC0
#define CAN_SERVIZI                 Can0

#define AIRcc                       48
#define AIRGnd                      49
#define PRE                         47
#define BUZZER                      52
#define AIRB                        14
#define RTDB                        15
#define FAN                         9

#define INVERTER_ANALOG_PIN         DAC0
#define BRAKE_REGEN_PIN				DAC1

#define TPS1_PIN                    A0
#define TPS1_ADC_CHAN_NUM           ADC_CHER_CH7   // AD7

#define TPS2_PIN                    A1
#define TPS2_ADC_CHAN_NUM           ADC_CHER_CH6   // AD6

#define BRAKE_PIN                   A2
#define BRAKE_ADC_CHAN_NUM          ADC_CHER_CH5   // AD5

#define SC_PIN              		A3
#define SC_ADC_CHAN_NUM				ADC_CHER_CH4   // AD4

#endif /* _BOARD_PINOUT_H_ */