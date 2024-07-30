//
// Created by NewTToNN on 29.07.2024.
//

#ifndef ESP32_TEST_MOTOR_HPP
#define ESP32_TEST_MOTOR_HPP

#include <Arduino.h>

class Encoder {

public:

    const uint8_t PIN_A;
    const uint8_t PIN_B;

    volatile int32_t ticks = 0;

    Encoder(uint8_t a, uint8_t b);

    void init();

};


class L293NMotor {
private:
    const uint8_t DIR_A;
    const uint8_t DIR_B;

public:
    L293NMotor(uint8_t dir_a, uint8_t dir_b);

    /// установить ШИМ и направление
    void set(int16_t pwm_dir) const;

};

#endif //ESP32_TEST_MOTOR_HPP
