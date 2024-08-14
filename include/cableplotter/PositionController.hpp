#pragma once

#include "hardware/MotorRegulator.hpp"

namespace cableplotter {
    class PositionController {

    public:
        hardware::MotorRegulator left_regulator;
        hardware::MotorRegulator right_regulator;

        float ticks_in_mm{0};
        int canvas_width{0};
        int canvas_height{0};
        int left_offset{0};
        int right_offset{0};

        explicit PositionController(
                hardware::MotorRegulator &&leftRegulator,
                hardware::MotorRegulator &&rightRegulator
        ) : left_regulator(leftRegulator), right_regulator(rightRegulator) {}

        void setTarget(int x, int y) {
            int i = (canvas_height / 2) - y;
            int j = canvas_width / 2;
            left_regulator.setTarget(long((std::hypot(x + j, i) - left_offset) * ticks_in_mm));
            right_regulator.setTarget(long((std::hypot(x - j, i) - right_offset) * ticks_in_mm));
        }
    };
}