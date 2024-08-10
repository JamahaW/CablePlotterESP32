#pragma once

#include "Event.hpp"
#include "Item.hpp"
#include "Widget.hpp"

namespace ui {
    class Window;

    class Page {

    private:

        class PageSetter : public Widget {
            Page *target;
            Window &window;

        public:
            explicit PageSetter(Page *target, Window &window);
        };

        int cursor = 0;
        Window &window;
        std::vector<Item *> items;
        const char *title;
        PageSetter to_this_page;

    public:
        explicit Page(ui::Window &window, const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(Event e);

        Page *addPage(const char *child_title);

        void addItem(Item *w);


    private:

        void moveCursor(int delta);
    };
}