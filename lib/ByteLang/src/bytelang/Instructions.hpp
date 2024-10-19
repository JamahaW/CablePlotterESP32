#pragma once

#include <bytelang/Result.hpp>
#include <bytelang/Reader.hpp>

#include "cableplotter/Device.hpp"


namespace instructions {

    /// завершить работу
    bytelang::Result quit(
            bytelang::Reader &,
            cableplotter::Device &
    );

    /// Временная задержка
    bytelang::Result delay_ms(
            bytelang::Reader &,
            cableplotter::Device &
    );

    /// Установить скорость перемещения
    bytelang::Result set_speed(
            bytelang::Reader &,
            cableplotter::Device &
    );

    /// Установить множитель скорости
    bytelang::Result set_speed_multiplication(
            bytelang::Reader &,
            cableplotter::Device &
    );

    /// Установить значение прогресса
    bytelang::Result set_progress(
            bytelang::Reader &,
            cableplotter::Device &
    );

    /// Установить (Переместиться) позицию
    bytelang::Result set_position(
            bytelang::Reader &,
            cableplotter::Device &
    );

    /// Установить активный инструмент
    bytelang::Result set_active_tool(
            bytelang::Reader &,
            cableplotter::Device &
    );
}
