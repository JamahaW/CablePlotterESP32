#include <Arduino.h>
#include "hardware/MotorRegulator.hpp"

hardware::MotorRegulator::MotorRegulator(
        motor_regulator_config_t &state,
        Encoder &&encoder,
        MotorDriverL293 &&motor) :
        config(state), encoder(encoder),
        motor(motor) {}

int hardware::MotorRegulator::calcUDelta() {
    auto error = float(target - encoder.ticks);

    integral += error * config.pos_ki * config.d_time;
    integral = constrain(integral, -config.pos_max_abs_i, config.pos_max_abs_i);

    auto ret = int(error * config.pos_kp + integral);
    return constrain(ret, -delta, delta);
}

int hardware::MotorRegulator::calcUDirPWM() const {
    auto error = float(next - encoder.ticks);
    return int(error * config.pwm_kp);
}

void hardware::MotorRegulator::update() {
    next += calcUDelta();
    motor.set(calcUDirPWM());
}

void hardware::MotorRegulator::setDelta(char new_delta) {
    delta = constrain(new_delta, 1, config.d_ticks_max);
}

bool hardware::MotorRegulator::isReady() const {
    return abs(target - encoder.ticks) <= config.deviation;
}

void hardware::MotorRegulator::reset() {
    encoder.ticks = 0;
    next = 0;
    integral = 0.0F;
    target = 0;
    motor.set(0);
    delta = 0;
}


