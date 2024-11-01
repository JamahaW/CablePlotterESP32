#include <Wire.h>
#include "Arduino.h"

#include "gfx/OLED.hpp"


gfx::OLED display;

void setup() {
    Wire.begin(19, 23);
    display.init();
}

void loop() {
}