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
    public:

        using OnClickHandlerFunc = void(Widget &);
        using OnChangeHandlerFunc = void(Widget &, int);

    private:
        gfx::Font font = gfx::Font::SINGLE;
        uint8_t flags;
        const ValueType type;
        OnClickHandlerFunc *const on_click;

        OnChangeHandlerFunc *const on_change;

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

    using EventHandler = Event();

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

    public:
        const char *title;
        PageSetter to_this_page;

        explicit Page(ui::Window &window, const char *title);

        void render(gfx::OLED &display) const;

        bool handleInput(EventHandler *input);

        Page *addPage(const char *child_title) {
            auto p = new Page(window, child_title);
            widgets.push_back(&p->to_this_page);
            p->widgets.push_back(&to_this_page);
            return p;
        }

        Widget *addWidget(Widget *w, bool merged = false) {
            w->unbindFlags(StyleFlag::ISOLATED * merged);
            widgets.push_back(w);
            return w;
        }

    private:

        void moveCursor(int delta);

    };

    class Window {

    private:
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

    Widget *spinbox(int &value, int step, void (*on_spin)(Widget &) = nullptr);
}