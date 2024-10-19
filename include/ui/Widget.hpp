#pragma once

#include "Item.hpp"
#include <gfx/OLED.hpp>
#include <functional>

namespace ui {

    enum Style : unsigned char {
        CLEAN = 0,
        SQUARE_FRAMED = 1,
        TRIANGLE_FRAMED = 2,
        ARROW_PREFIX = 3,
    };

    enum class ValueType : unsigned char {
        CHARS = 0,
        INT = 1,
        FLOAT = 2,
    };

    class Widget : public Item {
    private:
        gfx::Font font{gfx::Font::SINGLE};
        Style style{Style::CLEAN};

        const ValueType type;
        const std::function<void(Widget *)> on_click;
        const std::function<void(Widget *, int)> on_change;

    public:
        void *value;

        explicit Widget(
                ValueType type,
                void *value,
                std::function<void(Widget *)> &&on_click = nullptr,
                std::function<void(Widget *, int)> &&on_change = nullptr
        );

        Widget *setStyle(Style new_style) {
            style = new_style;
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
    };
}


