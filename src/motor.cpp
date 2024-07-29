//
// Created by NewTToNN on 30.07.2024.
//

#include "motor.hpp"

Motor::Motor(uint8_t dir_a, uint8_t dir_b, uint8_t enc_a, uint8_t enc_b, void (*enc_handler)()) :
        DIR_A(dir_a), DIR_B(dir_b), ENC_B(enc_b) {
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
    pinMode(ENC_B, INPUT);
    attachInterrupt(digitalPinToInterrupt(enc_a), enc_handler, RISING);
}

void Motor::set(int16_t pwm_dir) const {
    const int pwm = abs(pwm_dir);
    analogWrite(DIR_A, (pwm_dir > 0) * pwm);
    analogWrite(DIR_B, (pwm_dir < 0) * pwm);
}
