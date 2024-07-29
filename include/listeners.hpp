//
// Created by NewTToNN on 28.07.2024.
//

#ifndef ESP32_TEST_LISTENERS_HPP
#define ESP32_TEST_LISTENERS_HPP

#include <EncButton.h>
#include <Arduino.h>

class ButtonListener {

private:

    VirtButton *button;
    const char *name;

    const char *getStateName() {
        switch (button->action()) {
            case EB_PRESS:
                return "Press";

            case EB_HOLD:
                return "Hold";

            case EB_STEP:
                return "Step";

            case EB_RELEASE:
                return "Release";

            case EB_CLICK:
                return "Click";

            default:
                return nullptr;
        }
    }

public:

    ButtonListener(const char *name, VirtButton *button) : button(button), name(name) {}

    void update() {
        const char *state_name = getStateName();
        if (state_name == nullptr) return;
        Serial.printf("Button:%s:%s:%d\n", name, state_name, button->action());
    }

};

#endif //ESP32_TEST_LISTENERS_HPP
