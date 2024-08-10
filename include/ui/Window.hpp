#pragma once

#include <functional>
#include "gfx/OLED.hpp"
#include "Page.hpp"
#include "Event.hpp"


namespace ui {

    class Window {

    private:
        std::function<Event()> input_handler;

    public:
        gfx::OLED &display;
        Page main_page;
        Page *current_page;

        explicit Window(gfx::OLED &display, std::function<Event()>&& input_handler);

        void update();
    };
}