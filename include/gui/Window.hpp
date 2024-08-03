#pragma once

#include <vector>
#include "pico/OLED.hpp"
#include "Widgets.hpp"

namespace gui {

    /// Интерфейс ввода
    class Input {
    public:

        enum class Event : char {
            /// Ничего не произошло
            IDLE,
            /// Клик на виджет
            CLICK,
            /// Выбрать следующий виджет
            NEXT,
            /// Выбрать предыдущий виджет
            PAST
        };

        virtual Event getEvent() const = 0;
    };

    class Window {

    private:
        unsigned short cursor = 0;
        bool display_update_require = true;
        pico::OLED &display;
        const Input &input;

    public:
        std::vector<Widget *> widgets;

        explicit Window(pico::OLED &display, Input &input) : display(display), input(input) {}

        void update() {
            updateInput();
            if (display_update_require) {
                display_update_require = false;
                updateDisplay();
            }
        }

    private:

        void updateInput() {
            switch (input.getEvent()) {
                case Input::Event::IDLE:
                    return;

                case Input::Event::CLICK:
                    display_update_require = true;
                    return;

                case Input::Event::NEXT:
                    display_update_require = true;
                    cursor--;
                    return;

                case Input::Event::PAST:
                    display_update_require = true;
                    cursor++;
                    return;
            }
        }

        void updateDisplay() const {
            display.setCursor(0, 0);
            for (auto i = 0; i < widgets.size(); i++) {
                widgets[i]->render(display, i == cursor);
            }
            display.clearAfterCursor();
        }
    };
}