#pragma once

#include <cstdint>
#include "pico/OLED.hpp"

namespace gui {

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

    class Widget final {
    public:

        using OnClickHandlerFunc = void(Widget &);
        using OnChangeHandlerFunc = void(Widget &, int);

    private:
        pico::Font font = pico::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        OnClickHandlerFunc *const on_click;

        OnChangeHandlerFunc *const on_change;

    public:
        int16_t config;

        const void *value;

        explicit Widget(uint8_t flags, ValueType type, void *value, void (*onClick)(Widget &),
                        void (*onChange)(Widget &, int), int16_t config);

        Widget *bindFlags(uint8_t on_flags) {
            flags |= on_flags;
            return this;
        }

        Widget *unbindFlags(uint8_t off_flags) {
            flags &= ~off_flags;
            return this;
        }

        Widget *setFont(pico::Font new_font) {
            font = new_font;
            return this;
        }

        void render(pico::OLED &display, bool selected) const;

        void onClick();

        void onChange(int change);

    private:
        void draw(pico::OLED &display) const;

        void drawFramed(pico::OLED &display, char begin, char end) const;
    };

    Widget *label(const char *title);

    Widget *button(const char *title, void (*callback)(Widget &));

    Widget *display(void *value, gui::ValueType type);

    Widget *spinbox(int &value, int step, void (*on_spin)(Widget &) = nullptr);
}