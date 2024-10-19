#pragma once


namespace constants {
    enum Pins : unsigned char {

        ///  Адаптер SD-карты
        PIN_SD_CS = 5,
        PIN_SD_MOSI = 23,
        PIN_SD_CLK = 18,
        PIN_SD_MISO = 19,

        ///  Энкодер-кнопка пользователя
        PIN_USER_ENCODER_A = 34,
        PIN_USER_ENCODER_B = 35,
        PIN_USER_ENCODER_BUTTON = 32,

        /// Левый регулятор мотора
        PIN_MOTOR_LEFT_ENCODER_A = 13, // Желтый провод
        PIN_MOTOR_LEFT_ENCODER_B = 14, // Зеленый провод
        PIN_MOTOR_LEFT_DRIVER_A = 33,
        PIN_MOTOR_LEFT_DRIVER_B = 25,
        // Красный провод -  OUT 4
        // Белый провод -    OUT 3

        /// Правый регулятор мотора
        PIN_MOTOR_RIGHT_ENCODER_A = 16, // Зеленый провод
        PIN_MOTOR_RIGHT_ENCODER_B = 17, // Желтый провод
        PIN_MOTOR_RIGHT_DRIVER_A = 26,
        PIN_MOTOR_RIGHT_DRIVER_B = 27,
        // Красный провод -  OUT 1
        // Белый провод -    OUT 2

        ///  Сервопривод смены инструмента
        PIN_SERVO_TURN = 4,
    };
}