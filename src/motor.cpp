#include <Arduino.h>
#include "motor.hpp"


L293NMotor::L293NMotor(uint8_t dir_a, uint8_t dir_b) : DIR_A(dir_a), DIR_B(dir_b) {
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
}

void L293NMotor::set(int pwm_dir) const {
    const int pwm = abs(pwm_dir);
    pwm_dir = constrain(pwm_dir, -255, 255);
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

MotorRegulator::MotorRegulator(motor_regulator_config_t &state, GA25Encoder &encoder, L293NMotor &motor)
        : config(state), encoder(encoder), motor(motor), timer(int(1000 * state.d_time)) {
}

int MotorRegulator::calcUDelta() {
    auto error = float(target - encoder.ticks);

    integral += error * config.pos_ki * config.d_time;
    integral = constrain(integral, -config.pos_max_abs_i, config.pos_max_abs_i);

    int ret = int(error * config.pos_kp + integral);
    ret = constrain(ret, -delta, delta);
    return ret;
}

void MotorRegulator::update() {
    if (not timer.isReady()) return;
    next += calcUDelta();
    motor.set(calcUDirPWM());
}

int MotorRegulator::calcUDirPWM() const {
    auto error = float(next - encoder.ticks);
    return int(error * config.pwm_kp);
}

void MotorRegulator::setDelta(int new_delta) {
    delta = constrain(new_delta, 1, config.d_ticks_max);
}

void MotorRegulator::setTarget(int new_target) {
    target = new_target;
}

bool MotorRegulator::isReady() const {
    return abs(target - encoder.ticks) <= config.deviation;
}

int MotorRegulator::getDelta() const {
    return delta;
}

int MotorRegulator::getTarget() const {
    return target;
}

