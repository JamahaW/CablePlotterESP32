#pragma once


namespace constants {
    enum Pins : unsigned char {

        ///  Адаптер SD-карты
        SD_CS = 5,
        SD_MOSI = 23,
        SD_CLK = 18,
        SD_MISO = 19,

        ///  Энкодер-кнопка пользователя
        USER_ENCODER_A = 34,
        USER_ENCODER_B = 35,
        USER_ENCODER_BUTTON = 32,

        /// Левый регулятор мотора
        MOTOR_LEFT_ENCODER_A = 13, // Желтый провод
        MOTOR_LEFT_ENCODER_B = 14, // Зеленый провод
        MOTOR_LEFT_DRIVER_A = 33,
        MOTOR_LEFT_DRIVER_B = 25,
        // Красный провод -  OUT 4
        // Белый провод -    OUT 3

        /// Правый регулятор мотора
        MOTOR_RIGHT_ENCODER_A = 16, // Зеленый провод
        MOTOR_RIGHT_ENCODER_B = 17, // Желтый провод
        MOTOR_RIGHT_DRIVER_A = 26,
        MOTOR_RIGHT_DRIVER_B = 27,
        // Красный провод -  OUT 1
        // Белый провод -    OUT 2

        ///  Сервопривод смены инструмента
        SERVO_TURN = 4,
    };
}