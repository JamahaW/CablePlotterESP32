#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include <constants/Pins.hpp>

#include <hardware/Encoder.hpp>

#include <bytelang/Device.hpp>
#include <bytelang/StreamInterpreter.hpp>

#include <gfx/OLED.hpp>

#include <ui/Widget.hpp>
#include <ui/Builders.hpp>
#include <ui/Factory.hpp>
#include <ui/FileWidget.hpp>
#include <ui/Window.hpp>


gfx::OLED display;

ui::Window &window = ui::Window::getInstance(display);

bytelang::Device &device = bytelang::Device::getInstance();

bytelang::StreamInterpreter &interpreter = bytelang::StreamInterpreter::getInstance();

// TODO убрать
ui::Page printing_page(window, "Printing");


[[noreturn]] static void printing_task(void *v) {
    const char *path = (const char *) v;

    Serial.printf("Opening: %s", path);

    fs::File bytecode_stream = SD.open(path);

    printing_page.title = path;
    window.setPage(&printing_page);

    if (bytecode_stream) {
        interpreter.run(bytecode_stream, device);
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

        if (not SD.begin(constants::PIN_SD_CS)) {
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

static void buildUI(ui::Page &mainPage) {
    ui_select_file(mainPage.addPage(" --=[ Media ]=--"));
    ui::build::positionControlPage(mainPage.addPage("PositionControl"), device.positionController);
    ui::build::motorRegulatorControlPage(mainPage.addPage("MotorLeft"), device.positionController.left_regulator);
    ui::build::motorRegulatorControlPage(mainPage.addPage("MotorRight"), device.positionController.right_regulator);
    ui::build::paintToolControlPage(mainPage.addPage("PainToolControl"), device.paintToolController);
    ui::build::printingPage(&printing_page, interpreter);
}

[[noreturn]] static void motor_regulators_task(void *) {
    device.positionController.canvas_height = 1200;
    device.positionController.canvas_width = 1200;

    device.positionController.left_regulator.setDelta(8);
    device.positionController.right_regulator.setDelta(8);

    device.positionController.left_regulator.encoder.attach();
    device.positionController.right_regulator.encoder.attach();

    const auto regulator_update_period = uint32_t(device.positionController.left_regulator.config.update_delta_ms * 1000);

    while (true) {
        device.positionController.update();
        delay(regulator_update_period);
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
    vTaskDelete(nullptr);
#pragma clang diagnostic pop
}

void setup() {
    analogWriteFrequency(30000);
    SPI.begin(constants::PIN_SD_CLK, constants::PIN_SD_MISO, constants::PIN_SD_MOSI, constants::PIN_SD_CS);

    Serial.begin(9600);
    Serial.println("HELLO WORLD");
    display.init();
    display.println("HELLO WORLD");

    buildUI(window.main_page);

    xTaskCreatePinnedToCore(motor_regulators_task, "pos_control", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    window.update();
    delay(1);
}
