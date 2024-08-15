#include <Arduino.h>
#include "gfx/OLED.hpp"
#include "ui/Page.hpp"
#include "ui/Window.hpp"
#include "ui/Widget.hpp"

ui::Page::Page(ui::Window &window, const char *title) :
        window(window),
        title(title),
        to_this_page(
                ValueType::CHARS,
                (void *) this->title,
                [&window, this](Widget *w) {
                    window.current_page = this;
                    window.display.clear();
                }
        ) {
    to_this_page.setStyle(ui::Style::ARROW_PREFIX);
}

void ui::Page::render(gfx::OLED &display) const {
    display.setCursor(0, 0);

    display.setFont(gfx::Font::SINGLE);
    display.println(title);

#define GUI_LAST_ITEM_INDEX 5 // TODO to gfx::OLED

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
        case Event::CLICK:
            items[cursor]->onClick();
            return true;
        case Event::NEXT_ITEM:
            moveCursor(1);
            return true;
        case Event::PAST_ITEM:
            moveCursor(-1);
            return true;
        case Event::CHANGE_UP:
            items[cursor]->onChange(1);
            return true;
        case Event::CHANGE_DOWN:
            items[cursor]->onChange(-1);
            return true;
        case Event::IDLE:
        default:
            return false;
    }
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


