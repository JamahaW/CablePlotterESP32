//
// Created by NewTToNN on 29.07.2024.
//

#ifndef ESP32_TEST_MOTOR_HPP
#define ESP32_TEST_MOTOR_HPP

#include <Arduino.h>

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


class MotorRegulator {
    int target_ticks = 0;

    int delta_ticks = 0;

    int next_ticks = 0;
    int u_delta_ticks = 0;
    int u_dir_pwm = 0;
};

#endif //ESP32_TEST_MOTOR_HPP
