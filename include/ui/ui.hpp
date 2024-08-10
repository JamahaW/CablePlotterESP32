#pragma once

#include <vector>
#include <esp_bit_defs.h>
#include "gfx/OLED.hpp"
#include "Item.hpp"

// TODO разбить по файлам

namespace ui {

    class Window;

    enum class Event : char {
        /// Ничего не произошло
        IDLE,
        /// Клик на виджет
        CLICK,
        /// Выбрать следующий виджет
        NEXT,
        /// Выбрать предыдущий виджет
        PAST,
        /// Изменение вверх
        CHANGE_UP,
        /// Изменение вниз
        CHANGE_DOWN,
    };

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

    class Window {

    private:
        using EventHandler = Event();
        EventHandler *const input_handler;

    public:
        gfx::OLED &display;
        Page main_page;
        Page *current_page;

        explicit Window(gfx::OLED &display, Event (*input_handler)());

        void update();
    };
}