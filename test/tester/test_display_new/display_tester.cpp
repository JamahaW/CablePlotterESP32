#include <Arduino.h>
#include <Adafruit_ST7735.h>


/*
LED     3V3
SCL     SCK
SDA     MOSI
A0      (any)
RESET   3V3
SC      (any)
GND     GND
VCC     3V3


SD_CS = 5,
SD_MOSI = 23,
SD_CLK = 18,
SD_MISO = 19,

 */

constexpr int Pin_LCD_CS = 5;

constexpr int Pin_LCD_A0 = 22;

constexpr int Pin_LCD_RST = -1;

constexpr int Pin_LCD_CLK = 18;

constexpr int Pin_LCD_MOSI = 23;

Adafruit_ST7735 lcd(Pin_LCD_CS, Pin_LCD_A0, Pin_LCD_MOSI, Pin_LCD_CLK, Pin_LCD_RST);

void setup() {
    lcd.initR();      // Init ST7735S chip, black tab
    lcd.enableDisplay(true);        // Enable display

    lcd.printf("Hello world!");

    delay(2000);
}

void loop() {
    delay(500);
    lcd.fillScreen(ST7735_BLACK);
    delay(500);
    lcd.fillScreen(ST7735_WHITE);
}