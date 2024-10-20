#include <bytelang/Device.hpp>

#include <constants/Pins.hpp>


bytelang::Device::Device(
        cableplotter::PaintToolController &&paint_tool_controller,
        cableplotter::PositionController &&position_controller
) :
        paintToolController(paint_tool_controller),
        positionController(position_controller) {
}

bytelang::Device &bytelang::Device::getInstance(const hardware::MotorRegulatorConfig &config) {
    static Device device(
            cableplotter::PaintToolController(
                    hardware::ServoController(constants::SERVO_TURN),
                    {
                            0,
                            0,
                            0,
                            0
                    }
            ),
            cableplotter::PositionController(
                    hardware::MotorRegulator(
                            config,
                            hardware::Encoder(constants::MOTOR_LEFT_ENCODER_A, constants::MOTOR_LEFT_ENCODER_B),
                            hardware::MotorDriverL293(constants::MOTOR_LEFT_DRIVER_A, constants::MOTOR_LEFT_DRIVER_B)
                    ),
                    hardware::MotorRegulator(
                            config,
                            hardware::Encoder(constants::MOTOR_RIGHT_ENCODER_A, constants::MOTOR_RIGHT_ENCODER_B),
                            hardware::MotorDriverL293(constants::MOTOR_RIGHT_DRIVER_A, constants::MOTOR_RIGHT_DRIVER_B)
                    )
            )
    );
    return device;
}
