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

MotorRegulator::MotorRegulator(motor_regulator_state_t &state, GA25Encoder &&encoder, L293NMotor &&motor)
        : state(state), encoder(encoder), motor(motor), timer(int(1000 * state.d_time)) {
}

int MotorRegulator::calcUDelta() {
    auto err = float(target - encoder.ticks);

    integral += err * state.pos_ki * state.d_time;
    integral = constrain(integral, -state.pos_max_i, state.pos_max_i);

    int ret = int(err * state.pos_kp + integral);
    ret = constrain(ret, -delta, delta);
    return ret;
}

void MotorRegulator::update() {
    if (not timer.ready()) return;

    next += calcUDelta();
    motor.set(calcUDirPWM());
}

int MotorRegulator::calcUDirPWM() const {
    auto error = float(next - encoder.ticks);
    return int(error * state.pwm_kp);
}

/*float ProportionalRegulator::calc(float error) const { return error * KP; }

ProportionalRegulator::ProportionalRegulator(const float kp) : KP(kp) {}

IntegralRegulator::IntegralRegulator(const float ki, const float dt, const float min, const float max)
        : dt(dt), min(min), max(max) {}

float IntegralRegulator::calc(float error) const {
    integral += error * dt;
    integral = constrain(integral, min, max);
    return integral * KI;
}*/
