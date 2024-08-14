#pragma once

namespace ui {
    enum class Event : char {
        /// Ничего не произошло
        IDLE,
        /// Клик на виджет
        CLICK,
        /// Выбрать следующий виджет
        NEXT_ITEM,
        /// Выбрать предыдущий виджет
        PAST_ITEM,
        /// Изменение вверх
        CHANGE_UP,
        /// Изменение вниз
        CHANGE_DOWN,
    };
}
