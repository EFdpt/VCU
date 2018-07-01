#include "rtds.h"
#include "board_pinout.h"

#include <Arduino.h>

__attribute__((__inline__))
void ready_to_drive_sound_start() {
    for (int count = 0; count < 4; count++) {
        delay(100);
        digitalWrite(BUZZER, HIGH);
        delay(270);
        digitalWrite(BUZZER, LOW);
    }
}