#include <Arduino.h>
#include <EncButton.h>
#include <Wire.h>

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

//#include "hardware/MotorRegulator.hpp"
//#include "hardware/Encoder.hpp"
//#include "hardware/MotorDriver.hpp"

// настройки регулятора общие для моторов
//hardware::motor_regulator_config_t motorRegulatorConfig = {
//        .d_time = 0.01F,
//        .pos_kp = 0.06F,
//        .pos_ki = 0.05F,
//        .pos_max_abs_i = 2.0F,
//        .pwm_kp = 2.0F,
//        .d_ticks_max = 17,
//        .deviation = 20,
//};
//
//hardware::MotorDriverL293 motor(12, 13);
//hardware::GA25Encoder encoder(14, 27);
//hardware::MotorRegulator regulator(motorRegulatorConfig, encoder, motor);



gfx::OLED display;
EncButton encoder(16, 17, 5);
ui::Window window(display, []() -> ui::Event {
    encoder.tick();
    if (encoder.left()) return ui::Event::NEXT;
    if (encoder.right()) return ui::Event::PAST;
    if (encoder.click()) return ui::Event::CLICK;
    if (encoder.leftH()) return ui::Event::CHANGE_UP;
    if (encoder.rightH()) return ui::Event::CHANGE_DOWN;
    return ui::Event::IDLE;
});

void calcConfig(ui::Page *p) {
    static int a = 0, b = 0, res = 0;
    auto re_calc = [](ui::Widget &) { res = a * b; };
    p->addWidget(ui::spinbox(a, 1, re_calc), true);
    p->addWidget(ui::label(" * "), true);
    p->addWidget(ui::spinbox(b, 1, re_calc), true);
    p->addWidget(ui::label(" = "), true);
    p->addWidget(ui::display(&res, ui::ValueType::INT));
}

void clickerConfig(ui::Page *p) {
    static int clicks = 0;
    using gfx::Font;
    p->addWidget(ui::display(&clicks, ui::ValueType::INT)->setFont(Font::DOUBLE_THIN));
    p->addWidget(ui::button("+", [](ui::Widget &) { clicks++; }), true)->setFont(Font::DOUBLE_WIDE);
    p->addWidget(ui::button("-", [](ui::Widget &) { clicks--; })->setFont(Font::DOUBLE_WIDE));
}

void buildUI() {
    ui::Page &mainPage = window.main_page;
    calcConfig(mainPage.addPage("calculator"));
    clickerConfig(mainPage.addPage("clicker"));
}

void setup() {
    analogWriteFrequency(30000);
    Wire.setClock(1000000UL);
    display.init();
    display.print("HELLO WORLD");
    buildUI();

//    regulator.encoder.attach(); // подключаю прерывания энкодера
//    regulator.setTarget(1000); // цель регулятора = 1000 тиков энкодера
//    regulator.setDelta(12);  // дельта = 12 тиков/dT
}

void loop() {
//    regulator.handleInput(); // обновление регулятора
    window.update();
}



