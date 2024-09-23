#pragma once

#include "hardware/MotorRegulator.hpp"
#include <cmath>

namespace cableplotter {
    class PositionController {

    public:
        hardware::MotorRegulator left_regulator;
        hardware::MotorRegulator right_regulator;

        int canvas_width{0};
        int canvas_height{0};

        explicit PositionController(
                hardware::MotorRegulator &&leftRegulator,
                hardware::MotorRegulator &&rightRegulator
        ) : left_regulator(leftRegulator), right_regulator(rightRegulator) {}

        void setTarget(int x, int y) const {
            int i = (canvas_height / 2) - y;
            int j = canvas_width / 2;

            left_regulator.setTarget(long(std::hypot(x + j, i)));
            right_regulator.setTarget(long(std::hypot(x - j, i)));
        }
    };
}