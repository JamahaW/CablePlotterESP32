#include <Arduino.h>
#include "EncButton.h"

#include "constants/Pins.hpp"

#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"


constexpr int MAX_PWM = 255;

constexpr int PWM_STEP = 5;

hardware::Encoder encoder_left(constants::Pins::MOTOR_LEFT_ENCODER_A, constants::Pins::MOTOR_LEFT_ENCODER_B);

hardware::Encoder encoder_right(constants::Pins::MOTOR_RIGHT_ENCODER_A, constants::Pins::MOTOR_RIGHT_ENCODER_B);

hardware::MotorDriverL293 driver_left(constants::Pins::MOTOR_LEFT_DRIVER_A, constants::Pins::MOTOR_LEFT_DRIVER_B);

hardware::MotorDriverL293 driver_right(constants::Pins::MOTOR_RIGHT_DRIVER_A, constants::Pins::MOTOR_RIGHT_DRIVER_B);

EncButton encoder(constants::USER_ENCODER_A, constants::USER_ENCODER_B, constants::USER_ENCODER_BUTTON);

void setup() {
    Serial.begin(9600);

    encoder_left.attach();
    encoder_right.attach();
}

hardware::MotorDriverL293 *target_driver = &driver_left;

hardware::Encoder *target_encoder = &encoder_left;

int pwm = 0;

void update_driver() {
    target_driver->set(pwm);
    pwm = constrain(pwm, -255, 255);
}

void loop() {
    delay(10);

    static const char *target_name = "left";

    encoder.tick();

    if (encoder.click()) {
        static bool target_flag = false;
        target_flag ^= 1;
        target_driver = (target_flag ? &driver_left : &driver_right);
        target_encoder = (target_flag ? &encoder_left : &encoder_right);
        target_name = (target_flag ? "Left" : "Right");
        pwm = 0;
    }

    if (encoder.right()) {
        pwm += PWM_STEP;
        update_driver();
    }

    if (encoder.left()) {
        pwm -= PWM_STEP;
        update_driver();

    }

    Serial.printf("%s: set: %d, pos:  %ld\n", target_name, pwm, target_encoder->ticks);
}