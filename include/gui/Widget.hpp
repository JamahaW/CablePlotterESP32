#pragma once

#include <cstdint>
#include "pico/OLED.hpp"

namespace gui {

    class Widget final {
    public:

        using OnClickHandlerFunc = void(Widget &);
        using OnChangeHandlerFunc = void(Widget &, int);

        enum StyleFlag : char {
            /// Виджет на отдельной строке
            ISOLATED = BIT(0),
            /// Виджет будет скрываться вне фокуса
            COMPACT = BIT(1),
            /// Обрамление []
            SQUARE_FRAMED = BIT(2),
            /// Обрамление <>
            TRIANGLE_FRAMED = BIT(3),
        };

        enum class ValueType : char {
            CHARS,
            INT,
            FLOAT,
        };

    private:
        uint8_t flags;
        const pico::Font font;
        const ValueType type;
        OnClickHandlerFunc *const on_click;

        OnChangeHandlerFunc *const on_change;

    public:

        const void *value;

        explicit Widget(uint8_t flags, pico::Font font, ValueType type, void *value,
                        void (*onClick)(Widget &) = nullptr,
                        void (*onChange)(Widget &, int) = nullptr);

        Widget &bindFlags(uint8_t on_flags) {
            flags |= on_flags;
            return *this;
        }

        Widget &unbindFlags(uint8_t off_flags) {
            flags &= ~off_flags;
            return *this;
        }

        void render(pico::OLED &display, bool selected) const;

        void onClick();

        void onChange(int change);

    private:
        void draw(pico::OLED &display) const;

        void drawFramed(pico::OLED &display, char begin, char end) const;
    };

    Widget label(const char *title, pico::Font font);

    Widget button(const char *title, void (*callback)(Widget &) = nullptr, pico::Font font = pico::Font::SINGLE);

    Widget display(void *value, Widget::ValueType type, pico::Font font = pico::Font::SINGLE);

    Widget spinbox(int *value, pico::Font font = pico::Font::SINGLE, void(*on_spin)(Widget &) = nullptr);
}