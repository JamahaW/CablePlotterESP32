#include <Arduino.h>
#include "MotorDriver.hpp"

hardware::MotorDriverL293::MotorDriverL293(uint8_t dir_a, uint8_t dir_b) : DIR_A(dir_a), DIR_B(dir_b) {
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
}

void hardware::MotorDriverL293::set(int pwm_dir) const {
    const int pwm = abs(pwm_dir);
    pwm_dir = constrain(pwm_dir, -255, 255);
    analogWrite(DIR_A, (pwm_dir > 0) * pwm);
    analogWrite(DIR_B, (pwm_dir < 0) * pwm);
}