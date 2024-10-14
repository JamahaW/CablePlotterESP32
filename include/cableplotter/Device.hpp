#pragma once

#include "cableplotter/PositionController.hpp"
#include "cableplotter/PaintToolController.hpp"
#include "bytelang/StreamInterpreter.hpp"


namespace cableplotter {
    class Device {
    public:
        explicit Device(
                PaintToolController &&paint_tool_controller,
                PositionController &&position_controller,
                bytelang::StreamInterpreter &&interpreter
        ) :
                paintToolController(paint_tool_controller),
                positionController(position_controller),
                interpreter(interpreter) {}

        PositionController positionController;
        PaintToolController paintToolController;
        bytelang::StreamInterpreter interpreter;

        bytelang::Result runStreamInterpreter(Stream &stream) {
            return interpreter.run(stream, *this);
        }
    };


}