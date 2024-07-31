#include <Arduino.h>
#include <EncButton.h>
#include <GyverOLED.h>

#include "motor.hpp"
#include "tools.hpp"

Button button_up(16);
Button button_down(17);


GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> display;


GA25Encoder encoder(14, 27);
L293NMotor motor(12, 13);

#define MOTOR_REGULATOR_DT 0.01F
#define MOTOR_POSITION_REGULATOR_KP 0.06F
#define MOTOR_POSITION_REGULATOR_KI 0.05F
#define MOTOR_POSITION_REGULATOR_MAX_I 2.0F

#define MOTOR_TICKS_REGULATOR_KP 2

plt::Timer motor_regulator_timer(int(1000 * MOTOR_REGULATOR_DT));


int u_output_pwm = 0;
int u_delta_ticks = 0;
int next_ticks = 0;
int delta_ticks = 15;
int target_position = 2000;

float motor_position_regulator_i = 0.0F;


void setup() {
    analogWriteFrequency(30000);
    Serial.begin(9600);
    encoder.attach();
    display.init();
    display.clear();
    Wire.setClock(1000000UL);
}

void updateMotorPWM() {

    int error = (next_ticks - encoder.ticks);
    u_output_pwm = error * MOTOR_TICKS_REGULATOR_KP;

    motor.set(u_output_pwm);
}


void updateMotorDeltaTicks() {
    auto err = float(target_position - encoder.ticks);

    motor_position_regulator_i += err * MOTOR_POSITION_REGULATOR_KI * MOTOR_REGULATOR_DT;
    motor_position_regulator_i = constrain(
            motor_position_regulator_i,
            -MOTOR_POSITION_REGULATOR_MAX_I,
            MOTOR_POSITION_REGULATOR_MAX_I);


    u_delta_ticks = int(err * MOTOR_POSITION_REGULATOR_KP + motor_position_regulator_i);
    u_delta_ticks = constrain(u_delta_ticks, -delta_ticks, delta_ticks);
}

bool motor_mode_flag = true;

void updateMotorValue(int8_t factor) {
#define MOTOR_MAX_DELTA_TICKS 17
    if (motor_mode_flag) {
        delta_ticks += factor;
        delta_ticks = constrain(delta_ticks, 1, MOTOR_MAX_DELTA_TICKS);
    } else {
        target_position += factor * 1000;
    }
}

void loop() {

    button_up.tick();
    button_down.tick();

    if (button_up.click() or button_up.step()) {
        updateMotorValue(1);
    }

    if (button_up.hold()) {
        motor_mode_flag ^= 1;
        display.setCursor(0, 0);
        display.printf("mode: %d", motor_mode_flag);
    }

    if (button_down.click() or button_down.step()) {
        updateMotorValue(-1);
    }

    if (motor_regulator_timer.ready()) {
        next_ticks += u_delta_ticks;
        updateMotorPWM();
        updateMotorDeltaTicks();
    }

    static plt::Timer info_log_timer(500);
    if (info_log_timer.ready()) {
        display.setCursor(0, 1);
        display.printf("ticks: %5d\n\r", encoder.ticks);
        display.printf("s_T: %5d\n\r", target_position);
        display.printf("s_DT: %3d\n\r", delta_ticks);
        display.printf("u_pwm: %5d\n\r", u_output_pwm);
        display.printf("u_DT: %5d\n\r", u_delta_ticks);
    }
}



