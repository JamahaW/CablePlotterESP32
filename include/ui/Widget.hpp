#pragma once

#include <cstdint>
#include "gfx/OLED.hpp"
#include "Page.hpp"

namespace ui {

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

    class Widget {
    public:

        using OnClickHandlerFunc = void(Widget &);
        using OnChangeHandlerFunc = void(Widget &, int);

    private:
        gfx::Font font = gfx::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        OnClickHandlerFunc *const on_click;

        OnChangeHandlerFunc *const on_change;

    public:
        int16_t config;

        const void *value;

        explicit Widget(uint8_t flags, ValueType type, void *value, void (*onClick)(Widget &) = nullptr,
                        void (*onChange)(Widget &, int) = nullptr, int16_t config = 0);

        Widget *bindFlags(uint8_t on_flags) {
            flags |= on_flags;
            return this;
        }

        Widget *unbindFlags(uint8_t off_flags) {
            flags &= ~off_flags;
            return this;
        }

        Widget *setFont(gfx::Font new_font) {
            font = new_font;
            return this;
        }

        void render(gfx::OLED &display, bool selected) const;

        void onClick();

        void onChange(int change);

    private:
        void draw(gfx::OLED &display) const;

        void drawFramed(gfx::OLED &display, char begin, char end) const;
    };

    Widget *label(const char *title);

    Widget *button(const char *title, void (*callback)(Widget &));

    Widget *pageSetter(Page *page);

    Widget *display(void *value, ui::ValueType type);

    Widget *spinbox(int &value, int step, void (*on_spin)(Widget &) = nullptr);
}