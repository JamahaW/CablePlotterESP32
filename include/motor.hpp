//
// Created by NewTToNN on 29.07.2024.
//

#ifndef ESP32_TEST_MOTOR_HPP
#define ESP32_TEST_MOTOR_HPP

#include <Arduino.h>
#include "tools.hpp"


class GA25Encoder {

public:
    const uint8_t PIN_A;
    const uint8_t PIN_B;

    /// Текущая позиция в тиках
    volatile mutable int32_t ticks = 0;

    GA25Encoder(uint8_t a, uint8_t b);

    /// Подключить обработчик прерывания
    void attach() const;
};


class L293NMotor {
private:
    const uint8_t DIR_A;
    const uint8_t DIR_B;

public:
    L293NMotor(uint8_t dir_a, uint8_t dir_b);

    /// установить ШИМ и направление
    void set(int32_t pwm_dir) const;
};


struct motor_regulator_state_t {
    const float d_time;
    const float pos_kp;
    const float pos_ki;
    const float pos_max_i;
    const float pwm_kp;
    const int d_ticks_max;
};

class MotorRegulator {
private:
    const motor_regulator_state_t &state;
    GA25Encoder &encoder;
    L293NMotor &motor;
    plt::Timer timer;
    mutable int next_ticks = 0;
    mutable float integral = 0.0F;

public:
    int target_ticks = 0;
    int delta_ticks = 0;

    explicit MotorRegulator(motor_regulator_state_t &state, GA25Encoder &encoder, L293NMotor &motor);

    void update();

private:
    int calcUDeltaTicks();

    int calcUDirPWM() const;
};

#endif //ESP32_TEST_MOTOR_HPP
