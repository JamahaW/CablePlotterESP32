#pragma once

#include "cableplotter/PositionController.hpp"
#include "cableplotter/PaintToolController.hpp"


namespace bytelang {
    class Device {
    public:
        explicit Device(
                cableplotter::PaintToolController &&paint_tool_controller,
                cableplotter::PositionController &&position_controller
        );

        static Device &getInstance(const hardware::MotorRegulatorConfig &config = hardware::MotorRegulatorConfig::getInstance());

        cableplotter::PositionController positionController;
        cableplotter::PaintToolController paintToolController;
    };
}
