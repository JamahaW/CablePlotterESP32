#pragma once

#include <cstdint>

namespace hardware {

    class MotorDriverL293 {
    private:
        const uint8_t DIR_A;
        const uint8_t DIR_B;

    public:
        explicit MotorDriverL293(uint8_t dir_a, uint8_t dir_b);

        /// установить ШИМ и направление
        void set(int32_t pwm_dir) const;
    };
}