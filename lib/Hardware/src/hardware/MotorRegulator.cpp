#include <Arduino.h>
#include "MotorRegulator.hpp"

hardware::MotorRegulator::MotorRegulator(
        motor_regulator_config_t &state,
        Encoder &&encoder,
        MotorDriverL293 &&motor) :
        config(state), encoder(encoder),
        motor(motor) {}

int hardware::MotorRegulator::calcUDelta() {
    auto error = float(target_ticks - encoder.ticks);

    integral += error * config.pos_ki * config.d_time;
    integral = constrain(integral, -config.pos_max_abs_i, config.pos_max_abs_i);

    auto ret = int(error * config.pos_kp + integral);
    return constrain(ret, -delta_ticks, delta_ticks);
}

int hardware::MotorRegulator::calcUDirPWM() const {
    auto error = float(next_position_ticks - encoder.ticks);
    return int(error * config.pwm_kp);
}

void hardware::MotorRegulator::update() {
    next_position_ticks += calcUDelta();
    motor.set(calcUDirPWM());
}

void hardware::MotorRegulator::setDelta(char new_delta) {
    delta_ticks = constrain(new_delta, 1, config.d_ticks_max);
}

bool hardware::MotorRegulator::isReady() const {
    return abs(target_ticks - encoder.ticks) <= config.deviation;
}

void hardware::MotorRegulator::reset() {
    encoder.ticks = 0;
    next_position_ticks = 0;
    integral = 0.0F;
    target_ticks = 0;
    motor.set(0);
    delta_ticks = 0;
}

long hardware::MotorRegulator::calcTicks(int distance_mm) const {
    return long(float(distance_mm - offset_mm) * config.ticks_in_mm);
}

void hardware::MotorRegulator::setCurrent(int distance_mm) const {
    next_position_ticks = encoder.ticks = calcTicks(distance_mm);
}

void hardware::MotorRegulator::setTarget(int distance_mm) const {
    target_ticks = calcTicks(distance_mm);
}

