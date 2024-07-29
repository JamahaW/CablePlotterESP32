//
// Created by NewTToNN on 29.07.2024.
//

#ifndef ESP32_TEST_MOTOR_HPP
#define ESP32_TEST_MOTOR_HPP

#include <Arduino.h>

class Motor {
private:
    using pin = const uint8_t;
    const uint8_t DIR_B, DIR_A, ENC_B;
    volatile mutable int32_t encoder_ticks = 0;

public:
    Motor(uint8_t dir_a, uint8_t dir_b, uint8_t enc_a, uint8_t enc_b, void (*enc_handler)());

    void set(int16_t pwm_dir) const;

    void _onEncoderInterrupt() {
        digitalRead(ENC_B) ? encoder_ticks++ : encoder_ticks--;
    }
};


#endif //ESP32_TEST_MOTOR_HPP
