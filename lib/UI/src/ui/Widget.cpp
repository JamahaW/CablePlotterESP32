#include <ui/Widget.hpp>


ui::Widget::Widget(
        ValueType type,
        void *value,
        std::function<void(Widget *)> &&on_click,
        std::function<void(Widget *, int)> &&on_change
) :
        type(type),
        value(value),
        on_change(on_change),
        on_click(on_click) {
}

static constexpr char style_begin[]{
        0,      // CLEAN
        '[',    // SQUARE_FRAMED
        '<',    // TRIANGLE_FRAMED
        '>',    // ARROW_PREFIX
};

static constexpr char style_end[]{
        0,      // CLEAN
        ']',    // SQUARE_FRAMED
        '>',    // TRIANGLE_FRAMED
        0       // ARROW_PREFIX
};

void ui::Widget::render(gfx::OLED &display, bool selected) const {
    display.setFont(font);
    display.setInvertText(selected);

    display.write(style_begin[style]);
    draw(display);
    display.write(style_end[style]);

    display.setInvertText(false);
}

void ui::Widget::onClick() {
    if (on_click != nullptr) { on_click(this); }
}

void ui::Widget::onChange(int change) {
    if (on_change != nullptr) { on_change(this, change); }
}

void ui::Widget::draw(gfx::OLED &display) const {
    if (value == nullptr) {
        display.print("null");
        return;
    }

    switch (type) {
        case ValueType::CHARS:
            display.print((const char *) value);
            return;

        case ValueType::INT:
            display.print(*(int *) value);
            return;

        case ValueType::FLOAT:
            display.print(*(float *) value);
            return;
    }
}