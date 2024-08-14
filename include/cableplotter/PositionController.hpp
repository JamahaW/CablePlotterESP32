#pragma once

#include "hardware/MotorRegulator.hpp"

namespace cableplotter {
    class PositionController {

    public:
        hardware::MotorRegulator left_regulator;
        hardware::MotorRegulator right_regulator;

        float ticks_in_mm{0};
        int width_mm{0};
        int height_mm{0};

        explicit PositionController(
                hardware::MotorRegulator &&leftRegulator,
                hardware::MotorRegulator &&rightRegulator
        ) : left_regulator(leftRegulator), right_regulator(rightRegulator) {}

        void setTarget(int x, int y) {
            int i = (height_mm / 2) - y;
            int j = width_mm / 2;
            left_regulator.setTarget(long(std::hypot(x + j, i) * ticks_in_mm));
            right_regulator.setTarget(long(std::hypot(x - j, i) * ticks_in_mm));
        }
    };
}