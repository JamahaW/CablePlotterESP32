#pragma once

#include <vector>
#include "gfx/OLED.hpp"
#include "Widget.hpp"

namespace ui {

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
            PAST,
            /// Изменение вверх
            CHANGE_UP,
            /// Изменение вниз
            CHANGE_DOWN,
        };

        virtual Event getEvent() const = 0;
    };

    class Window {

    private:
        unsigned short cursor = 0;
        gfx::OLED &display;
        const Input &input;

    public:
        std::vector<Widget *> widgets;

        explicit Window(gfx::OLED &display, Input &input) : display(display), input(input) {}

        void update() {
            if (updateInput()) {
                updateDisplay();
            }
        }

    private:

        bool updateInput() {
            switch (input.getEvent()) {
                case Input::Event::IDLE:
                    return false;

                case Input::Event::CLICK:
                    widgets[cursor]->onClick();
                    return true;

                case Input::Event::NEXT:
                    cursor--;
                    return true;

                case Input::Event::PAST:
                    cursor++;
                    return true;

                case Input::Event::CHANGE_UP:
                    widgets[cursor]->onChange(1);
                    return true;

                case Input::Event::CHANGE_DOWN:
                    widgets[cursor]->onChange(-1);
                    return true;
            }
            return false;
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