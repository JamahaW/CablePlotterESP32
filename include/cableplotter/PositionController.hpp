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

        void calcDistance(long &ret_left, long &ret_right, int x, int y) const {
            int i = canvas_height / 2 - y;
            int j = canvas_width / 2;

            ret_left = long(std::hypot(x + j, i));
            ret_right = long(std::hypot(x - j, i));
        }

        void setTarget(int x, int y) const {
            long left_distance_mm;
            long right_distance_mm;
            calcDistance(left_distance_mm, right_distance_mm, x, y);

            left_regulator.setTarget(left_distance_mm);
            right_regulator.setTarget(right_distance_mm);
        }
    };
}