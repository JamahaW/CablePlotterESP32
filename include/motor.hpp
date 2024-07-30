//
// Created by NewTToNN on 29.07.2024.
//

#ifndef ESP32_TEST_MOTOR_HPP
#define ESP32_TEST_MOTOR_HPP

#include <Arduino.h>

class L293NMotorEncoder {
private:
    const uint8_t DIR_B, DIR_A, ENC_B;

public:
    volatile int32_t encoder_ticks = 0;

    L293NMotorEncoder(uint8_t dir_a, uint8_t dir_b, uint8_t enc_a, uint8_t enc_b, void (*enc_handler)());


    /// установить ШИМ и направление
    void set(int16_t pwm_dir) const;

    /// Обработчик энкодера
    void _onEncoderInterrupt();
};


#endif //ESP32_TEST_MOTOR_HPP
