#pragma once

#include <cstdint>
#include "pico/OLED.hpp"

namespace gui {

    /// Базовый виджет
    class Widget {
    public:

        enum StyleFlag : char {
            /// Виджет на отдельной строке
            ISOLATED = BIT(0),
            /// Виджет будет скрываться вне фокуса
            COMPACT = BIT(1),
            /// Обрамление []
            SQUARE_FRAMED = BIT(2),
            /// Обрамление <>
            TRIANGLE_FRAMED = BIT(3),

            /// Стили по умолчанию
            LABEL_DEFAULT = ISOLATED,
            BUTTON_DEFAULT = SQUARE_FRAMED,
            VALUE_DISPLAY_DEFAULT = TRIANGLE_FRAMED
        };

        /// Флаги стилей виджета
        const uint8_t style;
        const pico::Font font;

        explicit Widget(const pico::Font font, uint8_t style) : font(font), style(style) {}

        /// Отрисовать виджет на странице
        void render(pico::OLED &display, bool selected) const {
            display.setFont(font);
            display.setInvertText(selected);

            if (style & StyleFlag::SQUARE_FRAMED) {
                drawFramed(display, selected, '[', ']');
            } else if (style & StyleFlag::TRIANGLE_FRAMED) {
                drawFramed(display, selected, '<', '>');
            } else {
                draw(display, selected);
            }

            display.setInvertText(false);
            display.write((style & StyleFlag::ISOLATED) ? '\n' : ' ');
        }

        /// Событие клика на виджет
        virtual void onClick() {}

        /// Событие изменения (для кнопок и ползунков)
        virtual void onChange() {}

    protected:
        /// Отрисовка виджета
        virtual void draw(pico::OLED &display, bool selected) const = 0;

    private:
        void drawFramed(pico::OLED &display, bool selected, char begin, char end) const {
            display.write(begin);
            draw(display, selected);
            display.write(end);
        }
    };

    /// Отображение текста
    class Label : public Widget {
    private:
        const char *title;

    public:
        explicit Label(const char *title, pico::Font font = pico::Font::SINGLE, uint8_t style = 0) :
                Widget(font, style ^ StyleFlag::LABEL_DEFAULT), title(title) {}

    protected:
        void draw(pico::OLED &display, bool selected) const override {
            display.print(title);
        }
    };

    /// Кнопка с обработчиком событий
    class Button : public Widget {
    private:
        const char *title;

        void (*callback)();

    public:
        explicit Button(const char *title, void (*callback)() = nullptr, uint8_t style = 0) :
                Widget(pico::Font::SINGLE, style ^ StyleFlag::BUTTON_DEFAULT),
                title(title),
                callback(callback) {}

        void onClick() override {
            if (callback != nullptr) callback();
        }

    protected:

        void draw(pico::OLED &display, bool selected) const override {
            if (selected or not(style & StyleFlag::COMPACT)) display.print(title);
            else display.print(title[0]);
        }
    };

    /// Отображение значения
    template<typename T>
    class ValueDisplay : public Widget {
    private:
        T *value;

    protected:
    public:
        explicit ValueDisplay(T *value, pico::Font font = pico::Font::SINGLE, uint8_t style = 0) :
                Widget(font, style ^ StyleFlag::VALUE_DISPLAY_DEFAULT), value(value) {}

    protected:
        void draw(pico::OLED &display, bool selected) const override {
            if (value == nullptr) display.print("null");
            else display.print(*value);
        }
    };
}