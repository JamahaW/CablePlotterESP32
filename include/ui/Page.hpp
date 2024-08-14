#pragma once

#include "Event.hpp"
#include "Item.hpp"
#include "Widget.hpp"

namespace ui {
    class Window;

    class Page {

    private:

        unsigned char cursor = 0;
        Window &window;
        std::vector<Item *> items;
        const char *title;
        Widget to_this_page;

    public:
        explicit Page(ui::Window &window, const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(Event e);

        Page *addPage(const char *child_title);

        void addItem(Item *w);


    private:

        void moveCursor(char delta);
    };
}