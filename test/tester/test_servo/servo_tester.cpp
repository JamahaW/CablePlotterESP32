#include <Arduino.h>
#include <EncButton.h>
#include "constants/Pins.hpp"
#include "hardware/ServoController.hpp"


EncButton encoder_left(constants::USER_ENCODER_A, constants::USER_ENCODER_B, constants::USER_ENCODER_BUTTON);

hardware::ServoController servo(constants::Pins::SERVO_TURN);

void setup() {
    Serial.begin(9600);
}

void loop() {
    static int target = 90;

    encoder_left.tick();

    if (encoder_left.left()) {
        target--;
        Serial.println("--");
        servo.setPosition(target);
    }

    if (encoder_left.right()) {
        target++;
        Serial.println("++");
        servo.setPosition(target);
    }

    if (encoder_left.click()) {
        static bool is_attached = false;

        is_attached ^= 1;

        if (is_attached) {
            servo.enable();
            Serial.println("enable");
        } else {
            servo.disable();
            Serial.println("disable");
        }
    }
}