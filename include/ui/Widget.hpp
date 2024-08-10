#pragma once

#include <cstdint>
#include <esp_bit_defs.h>
#include "Item.hpp"
#include "gfx/OLED.hpp"
#include <functional>

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

    class Widget : public Item {
    private:
        gfx::Font font = gfx::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        std::function<void(Widget *)> on_click;
        std::function<void(Widget *, int)> on_change;

    public:
        const void *value;

        explicit Widget(
                uint8_t flags,
                ValueType type,
                void *value,
                std::function<void(Widget *)> on_click = nullptr,
                std::function<void(Widget *, int)> on_change = nullptr
        );

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
}


