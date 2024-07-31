//
// Created by NewTToNN on 30.07.2024.
//

#include <Arduino.h>
#include "motor.hpp"


L293NMotor::L293NMotor(uint8_t dir_a, uint8_t dir_b) : DIR_A(dir_a), DIR_B(dir_b) {
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
}

void L293NMotor::set(int32_t pwm_dir) const {
    pwm_dir = constrain(pwm_dir, -255, 255);
    const int pwm = abs(pwm_dir);
    analogWrite(DIR_A, (pwm_dir > 0) * pwm);
    analogWrite(DIR_B, (pwm_dir < 0) * pwm);
}

GA25Encoder::GA25Encoder(uint8_t a, uint8_t b) : PIN_B(b), PIN_A(a) {
    pinMode(a, INPUT);
    pinMode(b, INPUT);
}

static void IRAM_ATTR encoder_process(void *v) {
    auto *e = (GA25Encoder *) v;

    if (digitalRead(e->PIN_B)) {
        e->ticks++;
    } else {
        e->ticks--;
    }
}

void GA25Encoder::attach() const {
    attachInterruptArg(PIN_A, encoder_process, (void *) this, RISING);
}

