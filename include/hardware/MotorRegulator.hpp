#pragma once

#include "tools/Timer.hpp"
#include "Encoder.hpp"
#include "MotorDriver.hpp"

namespace hardware {

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

    /// Регулятор мотора
    class MotorRegulator {

    private:
        const motor_regulator_config_t &config;
        mutable float integral = 0.0F;

        /// следующая позиция смещения
        mutable long next = 0;
        /// Дельта смещения в тиках энкодера за dt
        mutable char delta = 0;

    public:

        /// Целевая позиция регулятора в тиках энкодера
        mutable long target = 0;

        Encoder encoder;
        MotorDriverL293 motor;

        explicit MotorRegulator(motor_regulator_config_t &state, Encoder &&encoder, MotorDriverL293 &&motor);

        /// Обновить состояние регулятора. Вызывать циклично
        void update();

        void setDelta(char new_delta);

        void reset();

        /// Регулятор достиг цели
        bool isReady() const;

    private:
        int calcUDelta();

        int calcUDirPWM() const;
    };
}


