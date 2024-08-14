#pragma once

/* Используемые GPIO */

/* Энкодер-кнопка пользователя */
#define PIN_USER_ENCODER_A          34
#define PIN_USER_ENCODER_B          35
#define PIN_USER_ENCODER_BUTTON     32

/* Левый регулятор мотора */
#define PIN_MOTOR_LEFT_ENCODER_A    18 // Желтый провод
#define PIN_MOTOR_LEFT_ENCODER_B    19 // Зеленый провод
#define PIN_MOTOR_LEFT_DRIVER_A     33
#define PIN_MOTOR_LEFT_DRIVER_B     25
//      Красный провод -            OUT 4
//      Белый провод -              OUT 3

/* Правый регулятор мотора */
#define PIN_MOTOR_RIGHT_ENCODER_A   16 // Зеленый провод
#define PIN_MOTOR_RIGHT_ENCODER_B   17 // Желтый провод
#define PIN_MOTOR_RIGHT_DRIVER_A    26
#define PIN_MOTOR_RIGHT_DRIVER_B    27
//      Красный провод -            OUT 1
//      Белый провод -              OUT 2


/* Константы */

/// Количество тиков энкодера в одном миллиметре троса
#define CONST_TICKS_IN_MM           (-18.331805682859763F)
