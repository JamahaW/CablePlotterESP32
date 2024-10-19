#pragma once

namespace bytelang {
    enum class Result : unsigned char {
        /// Прекращение работы виртуальной машины
        OK_EXIT,

        /// Продолжение работы виртуальной машины
        OK,

        /// Приостановка пользователем
        FATAL_ABORT,

        /// Байт сдвига начала программы был неверен
        FATAL_ERROR_INVALID_OFFSET_VALUE,

        /// Неверный код инструкции
        FATAL_ERROR_INVALID_INSTRUCTION_CODE,
    };
}