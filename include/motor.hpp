//
// Created by NewTToNN on 29.07.2024.
//

#ifndef ESP32_TEST_MOTOR_HPP
#define ESP32_TEST_MOTOR_HPP

#include <Arduino.h>

struct Motor {
    const uint8_t PIN_DIR_A, PIN_DIR_B, PIN_SPEED;

    Motor(uint8_t speed, uint8_t dir_a, uint8_t dir_b) : PIN_DIR_A(dir_a), PIN_DIR_B(dir_b), PIN_SPEED(speed) {
        pinMode(PIN_DIR_A, OUTPUT);
        pinMode(PIN_DIR_B, OUTPUT);
        pinMode(PIN_SPEED, OUTPUT);
    }

    void setDirection(bool reverse) const {
        digitalWrite(PIN_DIR_A, reverse);
        digitalWrite(PIN_DIR_B, !reverse);
    }

    void setPWM(uint16_t pwm) const {
        analogWrite(PIN_SPEED, pwm);
    }

};

#endif //ESP32_TEST_MOTOR_HPP
