#include "main.hpp"

#include <Arduino.h>
#include <EncButton.h>

#include "hardware/MotorRegulator.hpp"
#include "hardware/MotorDriver.hpp"
#include "hardware/Encoder.hpp"

#include "cableplotter/PositionController.hpp"

#include "gfx/OLED.hpp"
#include "ui/ui.hpp"

#include "SD.h"

hardware::motor_regulator_config_t regulator_config = {
        .d_time = 0.01F,
        .pos_kp = 0.06F,
        .pos_ki = 0.05F,
        .pos_max_abs_i = 2.0F,
        .pwm_kp = 2.0F,
        .d_ticks_max = 17,
        .deviation = 20,
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
EncButton encoder(PIN_USER_ENCODER_A, PIN_USER_ENCODER_B, PIN_USER_ENCODER_BUTTON);
ui::Window window(display, []() -> ui::Event {
    using ui::Event;
    encoder.tick();
    if (encoder.left()) return Event::NEXT_ITEM;
    if (encoder.right()) return Event::PAST_ITEM;
    if (encoder.click()) return Event::CLICK;
    if (encoder.leftH()) return Event::CHANGE_UP;
    if (encoder.rightH()) return Event::CHANGE_DOWN;
    return Event::IDLE;
});

void motorPageConfig(ui::Page *p, hardware::MotorRegulator &regulator) {
    static ui::Widget *L1 = ui::label("target/delta");
    static ui::Widget *pos_label = ui::label("ticks: ");

    p->addItem(L1);

    p->addItem(ui::spinbox(new int(0), 500, [&regulator](ui::Widget *w) {
        regulator.target = (*(int *) w->value);
    }, 50000, -50000));

    p->addItem(ui::spinbox(new int(0), 1, [&regulator](ui::Widget *w) {
        regulator.setDelta(char(*(int *) w->value));
    }, regulator_config.d_ticks_max));

    p->addItem(new ui::Group(
            {
                    pos_label,
                    ui::display((void *) &regulator.encoder.ticks, ui::ValueType::INT),
            }
    ));
}

ui::Item *makeVector2iSetter(ui::Page *p, const char *title, ui::Widget *x_spinbox, ui::Widget *y_spinbox) {
    static ui::Widget *x_label = ui::label("x:");
    static ui::Widget *y_label = ui::label("y:");
    p->addItem(ui::label(title));
    return new ui::Group({x_label, x_spinbox, y_label, y_spinbox});
}

ui::Widget *makePositionSpinbox(int *value) {
    constexpr int STEP = 50;
    constexpr int MAX_DIST_MM = 1500;
    return ui::spinbox(value, STEP, nullptr, MAX_DIST_MM, -MAX_DIST_MM);
}

ui::Item *makeNamedSpinbox(const char *title, ui::Widget *spinbox) {
    return new ui::Group({ui::label(title), spinbox});
}

void positionRegulatorPageConfig(ui::Page *p) {
    static int target_x = 0;
    static int target_y = 0;
    auto update_position = [](ui::Widget *) { positionController.setTarget(target_x, target_y); };

    p->addItem(makeVector2iSetter(
            p, "target",
            makePositionSpinbox(&target_x),
            makePositionSpinbox(&target_y)
    ));

    p->addItem(ui::button("run", update_position));

    p->addItem(makeNamedSpinbox("delta", ui::spinbox(new int(5), 1, [](ui::Widget *w) {
        auto v = char(*(int *) (w->value));
        positionController.left_regulator.setDelta(v);
        positionController.right_regulator.setDelta(v);
    }, regulator_config.d_ticks_max)));

    p->addItem(makeVector2iSetter(
            p, "delta L-X, R-Y",
            ui::spinbox(&positionController.left_offset, 5, update_position, 200, -200),
            ui::spinbox(&positionController.right_offset, 5, update_position, 200, -200)
    ));

    p->addItem(ui::button("reset ticks", [](ui::Widget *) {
        positionController.right_regulator.reset();
        positionController.left_regulator.reset();
    }));

    p->addItem(makeVector2iSetter(
            p, "canvas",
            makePositionSpinbox(&positionController.canvas_width),
            makePositionSpinbox(&positionController.canvas_height)
    ));

    p->addItem(ui::spinboxF(&positionController.ticks_in_mm, 5, 10000));
}

void buildUI() {
    ui::Page &mainPage = window.main_page;
    positionRegulatorPageConfig(mainPage.addPage("PositionController"));
    motorPageConfig(mainPage.addPage("motor Left"), positionController.left_regulator);
    motorPageConfig(mainPage.addPage("motor Right"), positionController.right_regulator);
}

[[noreturn]] void regulatorUpdateTask(void *) {
    positionController.left_regulator.encoder.attach();
    positionController.right_regulator.encoder.attach();
    positionController.left_regulator.setDelta(5);
    positionController.right_regulator.setDelta(5);
    positionController.ticks_in_mm = CONST_TICKS_IN_MM;
    positionController.canvas_height = 1200;
    positionController.canvas_width = 1200;

    while (true) {
        positionController.left_regulator.update();
        positionController.right_regulator.update();
        vTaskDelay(pdMS_TO_TICKS(regulator_config.d_time * 1000));
    }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"
    vTaskDelete(nullptr);
#pragma clang diagnostic pop
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char *path) {
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path) {
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path) {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path) {
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char *path) {
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void setup_sd() {
    while (!Serial) {
        delay(10);
    }

#define CD_CS   5
#define CD_MOSI 21
#define CD_CLK  18
#define CD_MISO 19


    SPI.begin(CD_CLK, CD_MISO, CD_MOSI, CD_CS);
    if (!SD.begin(CD_CS)) {

        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void setup() {
    analogWriteFrequency(30000);

    Serial.begin(9600);
    Serial.println("HELLO WORLD");

    display.init();
    display.println("ЗДРАВСТВУЙ, МИР!");

    setup_sd();


    buildUI();

    xTaskCreatePinnedToCore(regulatorUpdateTask, "pos_control", 4096, nullptr, 0, nullptr, 0);
}

void loop() {
    window.update();
}
