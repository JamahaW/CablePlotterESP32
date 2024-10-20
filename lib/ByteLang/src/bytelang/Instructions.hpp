#pragma once

#include <bytelang/Result.hpp>
#include <bytelang/Reader.hpp>
#include <bytelang/Device.hpp>


namespace instructions {

    /// завершить работу
    bytelang::Result quit(
            bytelang::Reader &,
            bytelang::Device &
    );

    /// Временная задержка
    bytelang::Result delay_ms(
            bytelang::Reader &,
            bytelang::Device &
    );

    /// Установить скорость перемещения
    bytelang::Result set_speed(
            bytelang::Reader &,
            bytelang::Device &
    );

    /// Установить множитель скорости
    bytelang::Result set_speed_multiplication(
            bytelang::Reader &,
            bytelang::Device &
    );

    /// Установить значение прогресса
    bytelang::Result set_progress(
            bytelang::Reader &,
            bytelang::Device &
    );

    /// Установить (Переместиться) позицию
    bytelang::Result set_position(
            bytelang::Reader &,
            bytelang::Device &
    );

    /// Установить активный инструмент
    bytelang::Result set_active_tool(
            bytelang::Reader &,
            bytelang::Device &
    );
}
