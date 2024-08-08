#include "ui/ui.hpp"
#include <Arduino.h>


ui::Page::Page(ui::Window &window, const char *title) : window(window), title(title), to_this_page(this, window) {}

void ui::Page::render(gfx::OLED &display) const {
    display.setCursor(0, 0);

    display.setFont(gfx::Font::SINGLE);
    display.println(title);

    for (auto i = 0; i < widgets.size(); i++) {
        widgets[i]->render(display, i == cursor);
    }

    display.clearAfterCursor();
}

bool ui::Page::handleInput(ui::Event e) {
    switch (e) {
        case Event::IDLE:
            return false;

        case Event::CLICK:
            widgets[cursor]->onClick();
            return true;

        case Event::NEXT:
            moveCursor(1);
            return true;

        case Event::PAST:
            moveCursor(-1);
            return true;

        case Event::CHANGE_UP:
            widgets[cursor]->onChange(1);
            return true;

        case Event::CHANGE_DOWN:
            widgets[cursor]->onChange(-1);
            return true;
    }
    return false;
}

void ui::Page::moveCursor(int delta) {
    cursor = constrain(cursor + delta, 0, widgets.size() - 1);
}

ui::Page *ui::Page::addPage(const char *child_title) {
    auto p = new Page(window, child_title);
    widgets.push_back(&p->to_this_page);
    p->widgets.push_back(&to_this_page);
    return p;
}

ui::Widget *ui::Page::addWidget(ui::Widget *w, bool merged) {
    w->unbindFlags(StyleFlag::ISOLATED * merged);
    widgets.push_back(w);
    return w;
}


ui::Page::PageSetter::PageSetter(ui::Page *target, Window &window) :
        ui::Widget(
                ui::ISOLATED | ui::SQUARE_FRAMED,
                ui::ValueType::CHARS,
                (void *) target->title,
                [](ui::Widget &w) {
                    auto &p = (PageSetter & )(w);
                    p.window.current_page = p.target;
                    p.window.display.clear();
                }
        ),
        target(target),
        window(window) {}


ui::Widget::Widget(uint8_t flags, ValueType type, void *value, void (*onClick)(ui::Widget &),
                   void (*onChange)(ui::Widget &, int), int16_t config) :
        flags(flags),
        type(type),
        value(value),
        on_change(onChange),
        on_click(onClick),
        config(config) {}

void ui::Widget::render(gfx::OLED &display, bool selected) const {
    display.setFont(font);
    display.setInvertText(selected);

    if (not(flags & StyleFlag::COMPACT) or selected) {
        if (flags & StyleFlag::SQUARE_FRAMED) {
            drawFramed(display, '[', ']');
        } else if (flags & StyleFlag::TRIANGLE_FRAMED) {
            drawFramed(display, '<', '>');
        } else {
            draw(display);
        }
    } else {
        display.write('-');
    }

    display.setInvertText(false);
    display.write((flags & StyleFlag::ISOLATED) ? '\n' : ' ');
}

void ui::Widget::onClick() {
    if (on_click != nullptr) on_click(*this);
}

void ui::Widget::onChange(int change) {
    if (on_change != nullptr) on_change(*this, change);
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

void ui::Widget::drawFramed(gfx::OLED &display, char begin, char end) const {
    display.write(begin);
    draw(display);
    display.write(end);
}

ui::Widget *ui::button(const char *title, void (*callback)(ui::Widget &)) {
    return new Widget(
            StyleFlag::SQUARE_FRAMED | StyleFlag::ISOLATED,
            ValueType::CHARS,
            (void *) title,
            callback);
}

ui::Widget *ui::display(void *value, ui::ValueType type) {
    return new Widget(StyleFlag::ISOLATED, type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ui::ValueType::CHARS);
}

ui::Widget *ui::spinbox(int &value, int step, void (*on_spin)(ui::Widget &)) {
    return new Widget(
            StyleFlag::TRIANGLE_FRAMED | StyleFlag::ISOLATED,
            ValueType::INT,
            &value,
            on_spin,
            [](Widget &w, int c) {
                *(int *) w.value += c * w.config;
                w.onClick();
            }, int16_t(step)
    );
}

ui::Window::Window(gfx::OLED &display, ui::Event (*input)()) :
        display(display),
        main_page(*this, "Main"),
        current_page(&main_page),
        input(input) {}

void ui::Window::update() {
    if (current_page->handleInput(input())) {
        current_page->render(display);
    }
}
