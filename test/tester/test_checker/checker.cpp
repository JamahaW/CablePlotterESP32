#include <Arduino.h>


#include "hardware/MotorDriver.hpp"
#include "constants/Pins.hpp"


auto left_driver = hardware::MotorDriverL293(constants::MOTOR_LEFT_DRIVER_A, constants::MOTOR_LEFT_DRIVER_B);

auto right_driver = hardware::MotorDriverL293(constants::MOTOR_RIGHT_DRIVER_A, constants::MOTOR_RIGHT_DRIVER_B);

void setup() {
    Serial.begin(9600);
    Serial.setTimeout(100);

    left_driver.set(255);
    delay(1000);

    left_driver.set(-255);
    delay(1000);

    left_driver.set(0);
    delay(2000);

    right_driver.set(255);
    delay(1000);

    right_driver.set(-255);
    delay(1000);

    right_driver.set(0);
    delay(2000);
}

hardware::MotorDriverL293 *getDriverByCode() {
    auto code = char(Serial.read());

    switch (code) {
        case 'l': return &left_driver;
        case 'r': return &right_driver;
        default: return nullptr;
    }
}

int readValue() {
    auto sign = char(Serial.read());
    int value = Serial.parseInt();

    switch (sign) {
        case '+': return value;
        case '-': return -value;
        default: return 0;
    }
}

void loop() {
//    if (Serial.available() == 0) {
//        return;
//    }
//
//    auto driver = getDriverByCode();
//
//    if (driver == nullptr) {
//        return;
//    }
//
//    driver->set(readValue());
}
