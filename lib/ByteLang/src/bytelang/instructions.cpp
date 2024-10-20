#include <bytelang/Instructions.hpp>
#include <bytelang/Types.hpp>
#include "Arduino.h"


bytelang::Result instructions::quit(bytelang::Reader &, bytelang::Device &) {
    return bytelang::Result::OK_EXIT;
}

bytelang::Result instructions::delay_ms(bytelang::Reader &reader, bytelang::Device &) {
    bytelang::u16 duration_ms;

    reader.read(duration_ms);

    delay(duration_ms);

    return bytelang::Result::OK;
}

bytelang::Result instructions::set_speed(bytelang::Reader &reader, bytelang::Device &device) {
    bytelang::i8 speed;
    reader.read(speed);

    device.positionController.right_regulator.setDelta(speed);
    device.positionController.left_regulator.setDelta(speed);

    return bytelang::Result::OK;
}

bytelang::Result instructions::set_speed_multiplication(bytelang::Reader &, bytelang::Device &) {
    // TODO доделать

    return bytelang::Result::OK;
}

bytelang::Result instructions::set_progress(bytelang::Reader &, bytelang::Device &) {
    // TODO доделать

    return bytelang::Result::OK;
}

bytelang::Result instructions::set_position(bytelang::Reader &reader, bytelang::Device &device) {
    bytelang::i16 target_y;
    bytelang::i16 target_x;

    reader.read(target_x);
    reader.read(target_y);

    device.positionController.setTarget(target_x, target_y);

    while (not device.positionController.isReady()) {
        delay(1);
    }

    return bytelang::Result::OK;
}

bytelang::Result instructions::set_active_tool(bytelang::Reader &reader, bytelang::Device &device) {
    bytelang::u8 tool_index;

    reader.read(tool_index);

    device.paintToolController.setActiveTool(static_cast<cableplotter::PaintToolController::Tool>(tool_index));

    return bytelang::Result::OK;
}
