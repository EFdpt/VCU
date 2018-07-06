/** 
 *  @file           rtds.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          RTDS module implementation file
 */

#include "rtds.h"
#include "board_pinout.h"

#include <Arduino.h>

/** @addtogroup RTDS_group
 *  
 *  @{
 */

/**
 *  @brief      This function starts RTDS.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
__attribute__((__inline__))
void ready_to_drive_sound_start() {
    for (int count = 0; count < 4; count++) {
        delay(100);
        digitalWrite(BUZZER, HIGH);
        delay(270);
        digitalWrite(BUZZER, LOW);
    }
}

/**
 * @}
 */