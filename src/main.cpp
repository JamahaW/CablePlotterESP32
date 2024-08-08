#include <Arduino.h>
#include <EncButton.h>
#include <Wire.h>

//#include "hardware/MotorRegulator.hpp"
//#include "hardware/Encoder.hpp"
//#include "hardware/MotorDriver.hpp"
#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

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


EncButton encoder = EncButton(16, 17, 5);

ui::Window window(display, []() -> ui::Event {
    encoder.tick();

    if (encoder.left()) return ui::Event::NEXT;
    if (encoder.right()) return ui::Event::PAST;
    if (encoder.click()) return ui::Event::CLICK;
    if (encoder.leftH()) return ui::Event::CHANGE_UP;
    if (encoder.rightH()) return ui::Event::CHANGE_DOWN;

    return ui::Event::IDLE;
});

void setup() {
    analogWriteFrequency(30000);
    Wire.setClock(1000000UL);

    display.init();
    display.print("HELLO WORLD");

    delay(200);

    using gfx::Font;

    ui::Page &mainPage = window.main_page;

    mainPage.addWidget(ui::label("Label in main_page"));
    mainPage.addPage("A");


//    auto main_page = new ui::Page("Main");
//    Widget *to_main_page = window.pageSetter(main_page);
//    window.current_page = main_page;
//
//    auto other_page = new ui::Page("Other Page");
//    Widget *to_other = window.pageSetter(other_page);
//    other_page->widgets.push_back(to_main_page);
//    main_page->widgets.push_back(to_other);
//
//    auto clicker = new ui::Page("Hamster Plotter");
//    Widget *to_clicker = window.pageSetter(clicker);
//    clicker->widgets.push_back(to_main_page);
//    main_page->widgets.push_back(to_clicker);

//    static int counter = 0;
//    clicker->widgets.push_back(display(&counter, ValueType::INT)->setFont(Font::DOUBLE_THIN));
//    clicker->widgets.push_back(button("+", [](Widget &) { counter++; })->unbindFlags(StyleFlag::ISOLATED)->setFont(Font::DOUBLE_WIDE));
//    clicker->widgets.push_back(button("-", [](Widget &) { counter--; })->setFont(Font::DOUBLE_WIDE));
//    clicker->widgets.push_back(spinbox(counter, 1000));

//    regulator.encoder.attach(); // подключаю прерывания энкодера
//    regulator.setTarget(1000); // цель регулятора = 1000 тиков энкодера
//    regulator.setDelta(12);  // дельта = 12 тиков/dT
}

void loop() {
//    regulator.handleInput(); // обновление регулятора
    window.update();
}



