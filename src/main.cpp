
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <EncButton.h>

#include <constants/common.hpp>

#include <hardware/MotorRegulator.hpp>
#include <hardware/MotorDriver.hpp>
#include <hardware/Encoder.hpp>

#include <hardware/ServoController.hpp>

#include <cableplotter/Device.hpp>

#include <gfx/OLED.hpp>
#include "ui/ui.hpp"
#include "ui/builders.hpp"

#include <bytelang/StreamInterpreter.hpp>
#include <bytelang/instructions.hpp>


hardware::motor_regulator_config_t regulator_config = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
        .ticks_in_mm = CONST_TICKS_IN_MM
};

cableplotter::Device device(
        cableplotter::PaintToolController(
                hardware::ServoController(PIN_SERVO_TURN),
                {
                        25,
                        0,
                        60,
                        120
                }
        ),
        cableplotter::PositionController(
                hardware::MotorRegulator(
                        regulator_config,
                        hardware::Encoder(PIN_MOTOR_LEFT_ENCODER_A, PIN_MOTOR_LEFT_ENCODER_B),
                        hardware::MotorDriverL293(PIN_MOTOR_LEFT_DRIVER_A, PIN_MOTOR_LEFT_DRIVER_B)
                ),
                hardware::MotorRegulator(
                        regulator_config,
                        hardware::Encoder(PIN_MOTOR_RIGHT_ENCODER_A, PIN_MOTOR_RIGHT_ENCODER_B),
                        hardware::MotorDriverL293(PIN_MOTOR_RIGHT_DRIVER_A, PIN_MOTOR_RIGHT_DRIVER_B)
                )
        ),
        bytelang::StreamInterpreter(
                {
                        instructions::quit,
                        instructions::delay_ms,
                        instructions::set_speed,
                        instructions::set_speed_multiplication,
                        instructions::set_progress,
                        instructions::set_position,
                        instructions::set_active_tool
                }
        )
);

gfx::OLED display;

ui::Window window(display, []() -> ui::Event {
    using ui::Event;

    static EncButton encoder(PIN_USER_ENCODER_A, PIN_USER_ENCODER_B, PIN_USER_ENCODER_BUTTON);

    encoder.tick();

    if (encoder.left()) { return Event::NEXT_ITEM; }
    if (encoder.right()) { return Event::PAST_ITEM; }
    if (encoder.click()) { return Event::CLICK; }
    if (encoder.leftH()) { return Event::CHANGE_UP; }
    if (encoder.rightH()) { return Event::CHANGE_DOWN; }

    return Event::IDLE;
});

// TODO убрать
ui::Page printing_page(window, "Printing");

// TODO перенести ui_printing в ui::builders
static void ui_printing(ui::Page *p) {
    p->addItem(ui::button("PAUSE", [](ui::Widget *w) {
        static bool p = false;
        device.interpreter.setPaused(p ^= 1);
        w->value = (void *) (p ? "RESUME" : "PAUSE");
    }));

    p->addItem(ui::button("ABORT", [](ui::Widget *) {
        device.interpreter.abort();
    }));

    p->addPage("Tune");
}

[[noreturn]] static void printing_task(void *v) {
    const char *path = (const char *) v;

    Serial.printf("Opening: %s", path);

    fs::File bytecode_stream = SD.open(path);

    printing_page.title = path;
    window.setPage(&printing_page);

    if (bytecode_stream) {
        device.runStreamInterpreter(bytecode_stream);
        bytecode_stream.close();
    }

    device.positionController.setTarget(0, 0);

    window.setPage(&window.main_page);

    vTaskDelete(nullptr);

    while (true) {
        delay(1);
    }
}

static void start_printing_task(ui::Widget *widget) {
    static String path;
    path = '/' + String((const char *) widget->value);
    xTaskCreate(printing_task, "printing", 4096, (void *) path.c_str(), 1, nullptr);
}

// TODO перенести ui_select_file в ui::builders
static void ui_select_file(ui::Page *p) {
    p->addItem(ui::button("reload", [p](ui::Widget *w) {
        display.clear();

        p->clearItems();
        p->addItem(w);

        if (not SD.begin(PIN_SD_CS)) {
            display.print("SD init not success");
            return;
        }

        fs::File root = SD.open("/");

        if (not root) {
            display.print("Root open error");
            return;
        }

        fs::File file = root.openNextFile();

        while (file) {
            if (not file.isDirectory()) {
                p->addItem(new ui::FileWidget(file, start_printing_task));
            }

            file.close();
            file = root.openNextFile();
        }

        file.close();
        root.close();
    }));
}

void buildUI(ui::Page &mainPage) {
    ui_select_file(mainPage.addPage(" --=[ Media ]=--"));
    ui::build::positionControlPage(mainPage.addPage("PositionControl"), device.positionController);
    ui::build::motorRegulatorControlPage(mainPage.addPage("MotorLeft"), device.positionController.left_regulator);
    ui::build::motorRegulatorControlPage(mainPage.addPage("MotorRight"), device.positionController.right_regulator);
    ui::build::paintToolControlPage(mainPage.addPage("PainToolControl"), device.paintToolController);

    ui_printing(&printing_page);
}

[[noreturn]] static void motor_regulators_task(void *) {
    device.positionController.canvas_height = 1200;
    device.positionController.canvas_width = 1200;

    device.positionController.left_regulator.setDelta(8);
    device.positionController.right_regulator.setDelta(8);

    device.positionController.left_regulator.encoder.attach();
    device.positionController.right_regulator.encoder.attach();

    const auto regulator_update_period = uint32_t(regulator_config.d_time * 1000);

    while (true) {
        device.positionController.update();
        delay(regulator_update_period);
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
    vTaskDelete(nullptr);
#pragma clang diagnostic pop
}

// TODO убрать Arduino
void setup() {
    analogWriteFrequency(30000);
    SPI.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    Serial.begin(9600);
    Serial.println("HELLO WORLD");
    display.init();
    display.println("HELLO WORLD");

    buildUI(window.main_page);

    xTaskCreatePinnedToCore(motor_regulators_task, "pos_control", 4096, nullptr, 0, nullptr, 0);
}

// TODO убрать Arduino
void loop() {
    window.update();
    delay(1);
}
