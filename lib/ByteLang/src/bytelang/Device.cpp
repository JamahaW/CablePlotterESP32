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
                    hardware::ServoController(constants::PIN_SERVO_TURN),
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
                            hardware::Encoder(constants::PIN_MOTOR_LEFT_ENCODER_A, constants::PIN_MOTOR_LEFT_ENCODER_B),
                            hardware::MotorDriverL293(constants::PIN_MOTOR_LEFT_DRIVER_A, constants::PIN_MOTOR_LEFT_DRIVER_B)
                    ),
                    hardware::MotorRegulator(
                            config,
                            hardware::Encoder(constants::PIN_MOTOR_RIGHT_ENCODER_A, constants::PIN_MOTOR_RIGHT_ENCODER_B),
                            hardware::MotorDriverL293(constants::PIN_MOTOR_RIGHT_DRIVER_A, constants::PIN_MOTOR_RIGHT_DRIVER_B)
                    )
            )
    );
    return device;
}
