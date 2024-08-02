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

    explicit GA25Encoder(uint8_t a, uint8_t b);

    /// Подключить обработчик прерывания
    void attach() const;
};


class L293NMotor {
private:
    const uint8_t DIR_A;
    const uint8_t DIR_B;

public:
    explicit L293NMotor(uint8_t dir_a, uint8_t dir_b);

    /// установить ШИМ и направление
    void set(int32_t pwm_dir) const;
};


struct motor_regulator_config_t {
    /// временная дельта обновления регулятора
    float d_time;

    /// P-коэффициент регулятора delta по положению
    float pos_kp;
    /// I-коэффициент регулятора delta по положению
    float pos_ki;
    /// Максимальное значение I-составляющей по модулю
    float pos_max_abs_i;

    /// P-коэффициент регулятора ШИМ по delta
    float pwm_kp;

    /// максимальное значение delta
    int d_ticks_max;
    /// максимальное отклонение, до которого регулятор считает, что он пришел к цели
    int deviation;
};


/// регулятор мотора
class MotorRegulator {
private:
    const motor_regulator_config_t &config;
    const plt::Timer timer;
    mutable float integral = 0.0F;

    /// следующая позиция смещения
    mutable int next = 0;
    /// Целевая позиция регулятора в тиках энкодера
    mutable int target = 0;
    /// Дельта смещения в тиках энкодера за dt
    mutable int delta = 0;

public:
    GA25Encoder &encoder;
    L293NMotor &motor;

    explicit MotorRegulator(motor_regulator_config_t &state, GA25Encoder &encoder, L293NMotor &motor);

    /// Обновить состояние регулятора. Вызывать циклично
    void update();

    /// Установить значение delta
    void setDelta(int new_delta);

    int getDelta() const;

    int getTarget() const;

    /// Установить целевую позицию
    void setTarget(int new_target);

    /// Регулятор достиг цели
    bool isReady() const;

private:
    int calcUDelta();

    int calcUDirPWM() const;
};

#endif //ESP32_TEST_MOTOR_HPP
