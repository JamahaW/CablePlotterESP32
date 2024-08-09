#include "ui/ui.hpp"
#include <Arduino.h>


ui::Page::Page(ui::Window &window, const char *title) : window(window), title(title), to_this_page(this, window) {}

void ui::Page::render(gfx::OLED &display) const {
    display.setCursor(0, 0);

    display.setFont(gfx::Font::SINGLE);
    display.println(title);

#define GUI_LAST_ITEM_INDEX 5

    uint8_t begin = max(cursor - GUI_LAST_ITEM_INDEX, 0);
    uint8_t end = _min(items.size(), GUI_LAST_ITEM_INDEX + 1) + begin;

    for (uint8_t i = begin; i < end; i++) {
        items[i]->render(display, i == cursor);
        display.println();
    }

    display.clearAfterCursor();
}


bool ui::Page::handleInput(ui::Event e) {
    switch (e) {
        case Event::IDLE:
            return false;

        case Event::CLICK:
            items[cursor]->onClick();
            return true;

        case Event::NEXT:
            moveCursor(1);
            return true;

        case Event::PAST:
            moveCursor(-1);
            return true;

        case Event::CHANGE_UP:
            items[cursor]->onChange(1);
            return true;

        case Event::CHANGE_DOWN:
            items[cursor]->onChange(-1);
            return true;
    }
    return false;
}

void ui::Page::moveCursor(int delta) {
    cursor = constrain(cursor + delta, 0, items.size() - 1);
}

ui::Page *ui::Page::addPage(const char *child_title) {
    auto p = new Page(window, child_title);
    items.push_back(&p->to_this_page);
    p->items.push_back(&to_this_page);
    return p;
}

void ui::Page::addItem(Item *w) {
    items.push_back(w);
}


ui::Page::PageSetter::PageSetter(ui::Page *target, Window &window) :
        ui::Widget(
                ui::ARROW_PREFIX,
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


ui::Widget::Widget(uint8_t flags, ValueType type, void *value, void (*on_click)(ui::Widget &),
                   void (*on_change)(ui::Widget &, int), int16_t config) :
        flags(flags),
        type(type),
        value(value),
        on_change(on_change),
        on_click(on_click),
        config(config) {}

void ui::Widget::render(gfx::OLED &display, bool selected) const {
    display.setFont(font);
    display.setInvertText(selected);

    if (not(flags & StyleFlag::COMPACT) or selected) {

        switch (flags) {
            case StyleFlag::SQUARE_FRAMED:
                drawFramed(display, '[', ']');
                break;
            case StyleFlag::TRIANGLE_FRAMED:
                drawFramed(display, '<', '>');
                break;
            case StyleFlag::ARROW_PREFIX:
                display.print("-> ");
            default:
                draw(display);
                break;
        }

    } else {
        display.write('-');
    }

    display.setInvertText(false);
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
            StyleFlag::SQUARE_FRAMED,
            ValueType::CHARS,
            (void *) title,
            callback);
}

ui::Widget *ui::display(void *value, ui::ValueType type) {
    return new Widget(0, type, value);
}

ui::Widget *ui::label(const char *title) {
    return ui::display((void *) title, ui::ValueType::CHARS);
}

ui::Widget *ui::spinbox(int *value, int step, void (*on_spin)(ui::Widget &)) {
    return new Widget(
            StyleFlag::TRIANGLE_FRAMED,
            ValueType::INT,
            value,
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

void ui::WidgetGroup::render(gfx::OLED &display, bool selected) const {
    for (int i = 0; i < widgets.size(); i++) {
        bool lit = (i == cursor) ^ (not control_inner) && selected;
        widgets[i]->render(display, lit);
        display.setInvertText(lit);
        display.write(' ');
    }
    display.setInvertText(false);
}

void ui::WidgetGroup::onClick() {
    if (control_inner) widgets[cursor]->onClick();
    control_inner ^= 1;
}

void ui::WidgetGroup::onChange(int change) {
    if (control_inner) {
        widgets[cursor]->onChange(change);
        return;
    }
    cursor = constrain(cursor + change, 0, widgets.size() - 1);
}

ui::WidgetGroup::WidgetGroup(const std::vector<Widget *> &widgets) : widgets(widgets) {}
