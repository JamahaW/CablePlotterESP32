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


struct motor_regulator_config_t {
    /// временная дельта обновления регулятора
    const float d_time;
    /// P-коэффициент регулятора delta по положению
    const float pos_kp;
    /// I-коэффициент регулятора delta по положению
    const float pos_ki;
    /// Максимальное значение I-составляющей по модулю
    const float pos_max_i;
    /// P-коэффициент регулятора ШИМ по delta
    const float pwm_kp;
    /// максимальное значение delta
    const int d_ticks_max;
};


/*class ProportionalRegulator {
private:
    const float KP;

public:
    explicit ProportionalRegulator(float kp);

    float calc(float error) const;
};

class IntegralRegulator {
public:
    explicit IntegralRegulator(float ki, float dt, float min, float max);

private:
    const float KI;
    const float dt;
    const float min;
    const float max;
    mutable float integral = 0.0F;

public:
    float calc(float error) const;
};*/

/// регулятор мотора
class MotorRegulator {
private:
    const motor_regulator_config_t &state;
    const plt::Timer timer;
    mutable int next = 0;
    mutable float integral = 0.0F;

public:
    GA25Encoder &encoder;
    L293NMotor &motor;
    /// Целевая позиция регулятора в тиках энкодера
    int target = 0;
    /// Дельта смещения в тиках энкодера за Timer::period
    int delta = 0;

    explicit MotorRegulator(motor_regulator_config_t &state, GA25Encoder &&encoder, L293NMotor &&motor);

    /// Обновить состояние регулятора. Вызывать циклично
    void update();

private:
    int calcUDelta();

    int calcUDirPWM() const;
};

#endif //ESP32_TEST_MOTOR_HPP
