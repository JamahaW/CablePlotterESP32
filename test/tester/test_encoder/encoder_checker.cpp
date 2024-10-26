#include <Arduino.h>
#include <EncButton.h>
#include "constants/Pins.hpp"


EncButton encoder(constants::USER_ENCODER_A, constants::USER_ENCODER_B, constants::USER_ENCODER_BUTTON);

void setup() {
    Serial.begin(9600);
}

void loop() {
    delay(10);

    encoder.tick();

    uint16_t state = encoder.action();

    if (state == 0) {
        return;
    }

    Serial.printf("State: %04X\n", state);
}