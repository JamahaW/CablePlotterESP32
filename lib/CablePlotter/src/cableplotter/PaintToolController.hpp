#pragma once

#include <hardware/ServoController.hpp>


namespace cableplotter {
    class PaintToolController {
    public:

        enum Tool : unsigned char {
            NONE = 0,
            LEFT,
            MIDDLE,
            RIGHT,
            TOOL_TOTAL_COUNT
        };

        using PositionArray = std::array<int, TOOL_TOTAL_COUNT>;

        hardware::ServoController servo_controller;

        PositionArray positions;

        explicit PaintToolController(hardware::ServoController &&servo, PositionArray &&positions)
                : servo_controller(servo),
                  positions(positions) {
        }

        void setActiveTool(Tool tool) {
            servo_controller.setPosition(positions.at(tool));
        }
    };
}