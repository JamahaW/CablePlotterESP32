#pragma once

#include <vector>
#include <esp_bit_defs.h>
#include "gfx/OLED.hpp"

namespace ui {
    enum StyleFlag : char {
        /// Виджет на отдельной строке
        ISOLATED = BIT(0),
        /// Виджет будет скрываться вне фокуса
        COMPACT = BIT(1),
        /// Обрамление []
        SQUARE_FRAMED = BIT(2),
        /// Обрамление <>
        TRIANGLE_FRAMED = BIT(3),
    };

    enum class ValueType : char {
        CHARS,
        INT,
        FLOAT,
    };

    class Widget {
    private:
        using OnClickHandler = void(Widget &);
        using OnChangeHandler = void(Widget &, int);

        gfx::Font font = gfx::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        OnClickHandler *const on_click;
        OnChangeHandler *const on_change;

    public:
        int16_t config;

        const void *value;

        explicit Widget(uint8_t flags, ValueType type, void *value, void (*onClick)(Widget &) = nullptr,
                        void (*onChange)(Widget &, int) = nullptr, int16_t config = 0);

        Widget *bindFlags(uint8_t on_flags) {
            flags |= on_flags;
            return this;
        }

        Widget *unbindFlags(uint8_t off_flags) {
            flags &= ~off_flags;
            return this;
        }

        Widget *setFont(gfx::Font new_font) {
            font = new_font;
            return this;
        }

        void render(gfx::OLED &display, bool selected) const;

        void onClick();

        void onChange(int change);

    private:
        void draw(gfx::OLED &display) const;

        void drawFramed(gfx::OLED &display, char begin, char end) const;

    };

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
        std::vector<Widget *> widgets;
        const char *title;
        PageSetter to_this_page;

    public:
        explicit Page(ui::Window &window, const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(Event e);

        Page *addPage(const char *child_title);

        Widget *addWidget(Widget *w, bool merged = false);

    private:

        void moveCursor(int delta);

    };

    class Window {

    private:
        using EventHandler = Event();
        EventHandler *const input;

    public:
        gfx::OLED &display;
        Page main_page;
        Page *current_page;

        explicit Window(gfx::OLED &display, Event (*input)());

        void update();
    };

    Widget *label(const char *title);

    Widget *button(const char *title, void (*callback)(Widget &));

    Widget *display(void *value, ui::ValueType type);

    Widget *spinbox(int &value, int step = 1, void (*on_spin)(Widget &) = nullptr);
}