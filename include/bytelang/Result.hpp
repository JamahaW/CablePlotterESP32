#pragma once

namespace bytelang {
    enum class Result : char {
        /// Прекращение работы виртуальной машины
        EXIT_OK,

        /// Продолжение работы виртуальной машины
        OK,

        /// Байт сдвига начала программы был неверен
        ERROR_INVALID_OFFSET_VALUE,

        /// Неверный код инструкции
        ERROR_INVALID_INSTRUCTION_CODE,
    };
}