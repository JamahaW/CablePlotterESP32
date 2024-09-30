#include "main.hpp"

#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <EncButton.h>

#include "hardware/MotorRegulator.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"

#include "cableplotter/PositionController.hpp"

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"
#include "ui/builders.hpp"

#include "bytelang/StreamInterpreter.hpp"
#include "bytelang/Types.hpp"

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

cableplotter::PositionController positionController(
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
);

gfx::OLED display;

ui::Window window(display, []() -> ui::Event {
    using ui::Event;

    static EncButton encoder(PIN_USER_ENCODER_A, PIN_USER_ENCODER_B, PIN_USER_ENCODER_BUTTON);

    encoder.tick();

    if (encoder.left()) return Event::NEXT_ITEM;
    if (encoder.right()) return Event::PAST_ITEM;
    if (encoder.click()) return Event::CLICK;
    if (encoder.leftH()) return Event::CHANGE_UP;
    if (encoder.rightH()) return Event::CHANGE_DOWN;

    return Event::IDLE;
});

ui::Page printing_page(window, "Printing");

static int progress = 0;

bytelang::Result vm_exit(bytelang::Reader &) {
    return bytelang::Result::EXIT_OK;
}

bytelang::Result vm_delay(bytelang::Reader &reader) {
    bytelang::u16 duration;
    reader.read(duration);

    delay(duration);

    return bytelang::Result::OK;
}

bytelang::Result vm_set_motors_speed(bytelang::Reader &reader) {
    bytelang::i8 delta_tick;

    reader.read(delta_tick);

    positionController.left_regulator.setDelta(delta_tick);
    positionController.right_regulator.setDelta(delta_tick);

    return bytelang::Result::OK;
}

bytelang::Result vm_set_progress(bytelang::Reader &reader) {
    bytelang::u8 v;
    reader.read(v);

    progress = v;

    return bytelang::Result::OK;
}

bytelang::Result vm_set_speed_multiplication(bytelang::Reader &reader) {
    bytelang::u16 multiplication;

    reader.read(multiplication);

    return bytelang::Result::OK;
}

bytelang::Result vm_move_to(bytelang::Reader &reader) {
    bytelang::i16 x;
    bytelang::i16 y;

    reader.read(x);
    reader.read(y);

    positionController.setTarget(x, y);

    while (not positionController.left_regulator.isReady() and not positionController.right_regulator.isReady()) {
        delay(1);
    }

    return bytelang::Result::OK;
}

bytelang::StreamInterpreter<7> interpreter(
        {
                vm_exit,
                vm_delay,
                vm_set_motors_speed,
                vm_set_progress,
                vm_set_speed_multiplication,
                vm_move_to,
        });

static void ui_printing(ui::Page *p) {
    p->addItem(new ui::Group({ui::label("Progress"), ui::display(&progress, ui::ValueType::INT)}));

    p->addItem(ui::button("PAUSE", [](ui::Widget *w) {
        static bool p = false;
        interpreter.setPaused(p ^= 1);
        w->value = (void *) (p ? "RESUME" : "PAUSE");
    }));

    p->addItem(ui::button("ABORT", [](ui::Widget *) {
        interpreter.abort();
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
        interpreter.run(bytecode_stream);
        bytecode_stream.close();
    }

    positionController.left_regulator.motor.set(0);
    positionController.right_regulator.motor.set(0);

    positionController.setTarget(0, 0);

    window.setPage(&window.main_page);

    vTaskDelete(nullptr);
    while (true) {}
}

static void start_printing_task(ui::Widget *widget) {
    static String path;
    path = '/' + String((const char *) widget->value);
    xTaskCreate(printing_task, "printing", 4096, (void *) path.c_str(), 1, nullptr);
}

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

void buildUI() {
    ui::Page &mainPage = window.main_page;

    ui_select_file(mainPage.addPage("PRINT"));
    ui::build::positionControlPage(mainPage.addPage("PositionController"), positionController);
    ui::build::motorRegulatorControlPage(mainPage.addPage("motor Left"), positionController.left_regulator);
    ui::build::motorRegulatorControlPage(mainPage.addPage("motor Right"), positionController.right_regulator);

    ui_printing(&printing_page);
}

[[noreturn]] static void motor_regulators_task(void *) {
    positionController.left_regulator.setDelta(8);
    positionController.right_regulator.setDelta(8);
    positionController.canvas_height = 1200;
    positionController.canvas_width = 1200;

    positionController.left_regulator.encoder.attach();
    positionController.right_regulator.encoder.attach();

    const auto regulator_update_period = uint32_t(regulator_config.d_time * 1000);

    while (true) {
        positionController.update();
        delay(regulator_update_period);
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
    vTaskDelete(nullptr);
#pragma clang diagnostic pop
}


void setup() {
    analogWriteFrequency(30000);
    SPI.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);

    Serial.begin(9600);
    Serial.println("HELLO WORLD");
    display.init();
    display.println("HELLO WORLD");

    buildUI();

    xTaskCreatePinnedToCore(motor_regulators_task, "pos_control", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    window.update(false);
}
