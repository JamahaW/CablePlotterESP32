#pragma once

#include <ui/Event.hpp>
#include <ui/Item.hpp>
#include <ui/Widget.hpp>


namespace ui {
    class Window;

    class Page {

    private:

        int cursor = 0;
        Window &window;
        std::vector<Item *> items;

    public:
        const char *title;

    private:
        Widget to_this_page;

    public:
        explicit Page(ui::Window &window, const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(Event e);

        Page *addPage(const char *child_title);

        void addItem(Item *w);

        void clearItems() {
            Item *to_parent_page = items[0];
            items.clear();
            items.push_back(to_parent_page);
        }


    private:

        void moveCursor(int delta);
    };
}