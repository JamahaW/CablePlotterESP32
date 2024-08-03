#include <Arduino.h>
#include <EncButton.h>
#include <Wire.h>

#include "hardware/MotorRegulator.hpp"
#include "hardware/Encoder.hpp"
#include "hardware/MotorDriver.hpp"
#include "pico/OLED.hpp"
#include "gui/Window.hpp"


// настройки регулятора общие для моторов
hardware::motor_regulator_config_t motorRegulatorConfig = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
};

hardware::MotorDriverL293 motor(12, 13);
hardware::GA25Encoder encoder(14, 27);
hardware::MotorRegulator regulator(motorRegulatorConfig, encoder, motor);

//auto c = [](int &p) { p++; };

class ButtonInput : public gui::Input {
private:
    mutable Button button_up = Button(16);
    mutable Button button_down = Button(17);

public:
    Event getEvent() const override {
        button_up.tick();
        button_down.tick();

        if (button_up.click()) return Event::NEXT;
        if (button_down.click()) return Event::PAST;
        if (button_up.hold()) return Event::CLICK;

        return Event::IDLE;
    }
};

/*
class TestManu {
    pico::OLED display;

    enum Modes {
        CHANGE_TARGET = 0,
        CHANGE_DELTA = 1,
        CHANGE_TARGET_STEP,
        LAST
    };

    int mode = Modes::CHANGE_DELTA;
    int step_shift = 0;

    void changeProcess(short factor) {
        switch (mode) {
            case CHANGE_TARGET:
                regulator.setTarget(regulator.getTarget() + factor * calcStep());
                break;

            case CHANGE_DELTA:
                regulator.setDelta(regulator.getDelta() + factor);
                break;

            case CHANGE_TARGET_STEP:
                step_shift += factor;
                step_shift = constrain(step_shift, 1, 16);
                break;

            case LAST:
                break;
        }
    }

public:
    void init() {
        display.init();
        display.setFont(pico::Font::SINGLE);
    }

    void update() {
        button_up.tick();
        button_down.tick();

        if (button_up.click() or button_up.step()) {
            changeProcess(1);
        }

        if (button_up.hold()) {
            mode = (mode + 1) % Modes::LAST;
            display.setCursor(0, 0);
            display.printf("mode: %d", mode);
        }

        if (button_down.click() or button_down.step()) {
            changeProcess(-1);
        }

        static tools::Timer info_log_timer(500);
        if (info_log_timer.isReady()) {
            display.setCursor(0, 1);
            display.printf("CurT: %5ld\n", regulator.encoder.ticks);
            display.printf("Target: %5ld\n", regulator.getTarget());
            display.printf("DeltaT: %3d\n", regulator.getDelta());
            display.printf("isReady: %d\n", regulator.isReady());
            display.printf("step: %5d\n", calcStep());
        }
    }

    int calcStep() const { return (1 << step_shift); }
} menu;
*/

pico::OLED display;
ButtonInput input;

gui::Window window(display, input);

void setup() {
    analogWriteFrequency(30000); // разогнал ШИМ чтобы не пищал
    Wire.setClock(1000000UL); // разогнал Wire чтоб дисплей не тормозил регулятор

    Serial.begin(9600);
    Serial.println("HELLO WORLD ESP32!");

    display.init();
    window.widgets.push_back(new gui::Label("Label"));
    window.widgets.push_back(new gui::Button("button"));
    window.widgets.push_back(new gui::Button("0_button", nullptr, gui::Widget::COMPACT));
    window.widgets.push_back(new gui::Button("1_button", nullptr, gui::Widget::COMPACT));
    window.widgets.push_back(new gui::ValueDisplay<int>(nullptr));


    regulator.encoder.attach(); // подключаю прерывания энкодера
//    regulator.setTarget(1000); // цель регулятора = 1000 тиков энкодера
//    regulator.setDelta(12);  // дельта = 12 тиков/dT
}


void loop() {
//    regulator.update(); // обновление регулятора
    window.update();
}



