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
        Input &input;

    public:
        std::vector<Widget *> widgets;

        explicit Window(pico::OLED &display, Input &input) : display(display), input(input) {}

        void update() {
            if (display_update_require) {
                display_update_require = false;
                updateDisplay();
            }
        }

    private:

        void updateDisplay() const {
            for (auto i = 0; i < widgets.size(); i++) {
                widgets[i]->render(display, i == cursor);
                display.clearAfterCursor();
            }
        }
    };
}