#pragma once

#include "gfx/OLED.hpp"
#include "Page.hpp"
#include "Event.hpp"


namespace ui {

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