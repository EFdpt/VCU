/** 
 *  @file           board_pinout.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          Board pinout module header
 */
#ifndef _BOARD_PINOUT_H_
#define _BOARD_PINOUT_H_

/** @addtogroup Board_model_group
 *  
 *  @{
 */

/**
 *  @def CAN_FUNZIONALE
 *  @brief Pin on board dedicated to CAN funzionale port
 */
#define CAN_FUNZIONALE              Can1

/**
 *  @def CAN_SERVIZI
 *  @brief Pin on board dedicated to CAN servizi port
 */
#define CAN_SERVIZI                 Can0

/**
 *  @def RTDB
 *  @brief Pin on board dedicated to RTD button
 */
#define RTDB                        15

/**
 *  @def FAN
 *  @brief Pin on board dedicated to FAN
 */
#define FAN                         9

/**
 *  @def INVERTER_ANALOG_PIN
 *  @brief Pin on board dedicated to inverter torque request analog signal
 */
#define INVERTER_ANALOG_PIN         DAC1

/**
 *  @def BRAKE_REGEN_PIN
 *  @brief Pin on board dedicated to inverter regen request analog signal
 */
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