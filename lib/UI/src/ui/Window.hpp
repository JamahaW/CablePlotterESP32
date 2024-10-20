#pragma once

#include <functional>
#include <EncButton.h>

#include <gfx/OLED.hpp>
#include <constants/Pins.hpp>

#include <ui/Page.hpp>
#include <ui/Event.hpp>


namespace ui {
    class Window {

    private:
        std::function<Event()> input_handler;

    public:
        gfx::OLED &display;
        Page main_page;
        Page *current_page;

        explicit Window(gfx::OLED &display, std::function<Event()> &&input_handler);

        static Window &getInstance(gfx::OLED &display);

        void update();

        void setPage(Page *new_page);
    };
}