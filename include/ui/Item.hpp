#pragma once

#include <vector>
#include <esp_bit_defs.h>
#include "gfx/OLED.hpp"

namespace ui {
    enum StyleFlag : char {
        /// Обрамление []
        SQUARE_FRAMED = 1,

        /// Обрамление <>
        TRIANGLE_FRAMED,

        /// Префикс стрелочка
        ARROW_PREFIX,

        /// Виджет будет скрываться вне фокуса
        COMPACT = BIT(6), // TODO отдельный флаг
    };
    enum class ValueType : char {
        CHARS,
        INT,
        FLOAT,
    };

    /// Элемент, который отображается на странице
    class Item {
    public:
        virtual void render(gfx::OLED &display, bool selected) const = 0;

        virtual void onClick() = 0;

        virtual void onChange(int change) = 0;
    };

    class Widget : public Item {
    private:
        using OnClickHandler = void(Widget &);
        using OnChangeHandler = void(Widget &, int);

        gfx::Font font = gfx::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        OnClickHandler *const on_click;
        OnChangeHandler *const on_change;

    public:
        int16_t config;

        const void *value;

        explicit Widget(uint8_t flags, ValueType type, void *value, OnClickHandler *on_click = nullptr,
                        OnChangeHandler *on_change = nullptr, int16_t config = 0);

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

        void render(gfx::OLED &display, bool selected) const override;

        void onClick() override;

        void onChange(int change) override;

    private:
        void draw(gfx::OLED &display) const;

        void drawFramed(gfx::OLED &display, char begin, char end) const;
    };

    class WidgetGroup : public Item {

    private:
        bool control_inner = false;
        int cursor = 0;
        std::vector<Widget *> widgets;

    public:
        explicit WidgetGroup(const std::vector<Widget *> &widgets);

        void render(gfx::OLED &display, bool selected) const override;

        void onClick() override;

        void onChange(int change) override;
    };

    Widget *label(const char *title);

    Widget *button(const char *title, void (*callback)(Widget &));

    Widget *display(void *value, ValueType type);

    Widget *spinbox(int *value, int step = 1, void (*on_spin)(Widget &) = nullptr);
}