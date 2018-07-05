#ifndef _BOARD_PINOUT_H_
#define _BOARD_PINOUT_H_

/** @addtogroup Board_model_group
 *  
 *  @{
 */

#define CAN_FUNZIONALE              Can1
#define CAN_SERVIZI                 Can0

#define AIRcc                       48
#define AIRGnd                      49
#define PRE                         47
#define BUZZER                      52
#define AIRB                        14
#define RTDB                        15
#define FAN                         9

#define INVERTER_ANALOG_PIN         DAC1
#define BRAKE_REGEN_PIN				DAC0

/**
 *  @def TPS1_PIN
 *  @brief Pin on board dedicated to first APPS (tps1)
 */
#define TPS1_PIN                    A0

/**
 *  @def TPS1_ADC_CHAN_NUM
 *  @brief GPIO pin on the Atmel SAM3X8E processor corresponding to tps1 signal (AD7)
 */
#define TPS1_ADC_CHAN_NUM           ADC_CHER_CH7

/**
 *  @def TPS2_PIN
 *  @brief Pin on board dedicated to second APPS (tps2)
 */
#define TPS2_PIN                    A1

/**
 *  @def TPS2_ADC_CHAN_NUM
 *  @brief GPIO pin on the Atmel SAM3X8E processor corresponding to tps2 signal (AD6)
 */
#define TPS2_ADC_CHAN_NUM           ADC_CHER_CH6

/**
 *  @def BRAKE_PIN
 *  @brief Pin on board dedicated to brake pedal position sensor
 */
#define BRAKE_PIN                   A2

/**
 *  @def BRAKE_ADC_CHAN_NUM
 *  @brief GPIO pin on the Atmel SAM3X8E processor corresponding to brake signal (AD5)
 */
#define BRAKE_ADC_CHAN_NUM          ADC_CHER_CH5

/**
 *  @def SC_PIN
 *  @brief Pin on board dedicated to SC read signal
 */
#define SC_PIN              		A3

/**
 *  @def SC_ADC_CHAN_NUM
 *  @brief GPIO pin on the Atmel SAM3X8E processor corresponding to SC signal (AD4)
 */
#define SC_ADC_CHAN_NUM				ADC_CHER_CH4

/**
 *  @}
 */

#endif /* _BOARD_PINOUT_H_ */