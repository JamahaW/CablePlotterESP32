#pragma once

namespace ui {
    /// Интерфейс ввода
    class Input {
    public:

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

        virtual Event getEvent() const = 0;
    };
}