#include <Arduino.h>


#include "hardware/MotorDriver.hpp"
#include "constants/Pins.hpp"


auto driver = hardware::MotorDriverL293(constants::MOTOR_LEFT_DRIVER_A, constants::MOTOR_LEFT_DRIVER_B);

void setup() {


    Serial.begin(9600);
    Serial.setTimeout(100);
}

void loop() {
    if (Serial.available() == 0) {
        return;
    }

    auto code = char(Serial.read());
    int value = Serial.parseInt();

    switch (code) {
        case '+':driver.set(value);
            break;

        case '-':driver.set(-value);
            break;

        case 'f':analogWriteFrequency(value);
            break;
    }
}
