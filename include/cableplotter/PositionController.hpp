#pragma once

#include "hardware/MotorRegulator.hpp"

namespace cableplotter {
    class PositionController {

    public:
        hardware::MotorRegulator left_regulator;
        hardware::MotorRegulator right_regulator;

        int canvas_height{0};
        int canvas_width{0};

        explicit PositionController(
                hardware::MotorRegulator &&leftRegulator,
                hardware::MotorRegulator &&rightRegulator
        ) : left_regulator(leftRegulator), right_regulator(rightRegulator) {}

        void setTarget(int x, int y) {}
    };
}