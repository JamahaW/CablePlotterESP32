#include <Arduino.h>
#include <EncButton.h>
#include "constants/Pins.hpp"


EncButton encoder_left(constants::USER_ENCODER_A, constants::USER_ENCODER_B, constants::USER_ENCODER_BUTTON);

void setup() {
    Serial.begin(9600);
}

void loop() {
//    delay(10);

    encoder_left.tick();

    uint16_t state = encoder_left.action();

    if (state == 0) {
        return;
    }

    Serial.printf("State: %04X\n", state);
}