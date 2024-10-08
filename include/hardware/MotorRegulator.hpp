#pragma once

#include "Encoder.hpp"
#include "MotorDriver.hpp"

namespace hardware {

    struct motor_regulator_config_t {
        /// временная дельта обновления регулятора
        float d_time;

        /// P-коэффициент регулятора delta_ticks по положению
        float pos_kp;
        /// I-коэффициент регулятора delta_ticks по положению
        float pos_ki;
        /// Максимальное значение I-составляющей по модулю
        float pos_max_abs_i;

        /// P-коэффициент регулятора ШИМ по delta_ticks
        float pwm_kp;

        /// максимальное значение delta_ticks
        int d_ticks_max;
        /// максимальное отклонение, до которого регулятор считает, что он пришел к цели
        int deviation;

        /// кол-во тиков в 1 мм
        float ticks_in_mm;
    };

    /// Регулятор мотора
    class MotorRegulator {

    public:
        const motor_regulator_config_t &config;

    private:
        mutable float integral{0.0F};

        /// следующая позиция смещения
        mutable long next_position_ticks{0};
        /// Дельта смещения в тиках энкодера за dt
        mutable char delta_ticks{0};

    public:

        /// Целевая позиция регулятора в тиках энкодера
        mutable long target_ticks{0};

        int offset_mm{0};

        Encoder encoder;
        MotorDriverL293 motor;

        explicit MotorRegulator(motor_regulator_config_t &state, Encoder &&encoder, MotorDriverL293 &&motor);

        /// Обновить состояние регулятора. Вызывать циклично
        void update();

        void setTarget(int distance_mm) const;

        void setCurrent(int distance_mm) const;

        void setDelta(char new_delta);

        void reset();

        /// Регулятор достиг цели
        bool isReady() const;

    private:
        int calcUDelta();

        int calcUDirPWM() const;

        long calcTicks(int distance_mm) const;
    };
}


