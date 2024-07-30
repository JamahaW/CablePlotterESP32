//
// Created by NewTToNN on 30.07.2024.
//

#include <Arduino.h>
#include "motor.hpp"


L293NMotor::L293NMotor(uint8_t dir_a, uint8_t dir_b) : DIR_A(dir_a), DIR_B(dir_b) {
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
}

void L293NMotor::set(int16_t pwm_dir) const {
    const int pwm = abs(pwm_dir);
    analogWrite(DIR_A, (pwm_dir > 0) * pwm);
    analogWrite(DIR_B, (pwm_dir < 0) * pwm);
}

Encoder::Encoder(uint8_t a, uint8_t b) : PIN_B(b), PIN_A(a) {
    pinMode(a, INPUT);
    pinMode(b, INPUT);
}

static void IRAM_ATTR encoder_int(void *v) {
    auto *e = (Encoder *) v;

    if (digitalRead(e->PIN_B)) {
        e->ticks++;
    } else {
        e->ticks--;
    }
}

void Encoder::init() {
    attachInterruptArg(PIN_A, encoder_int, this, RISING);
}
