#include <unity.h>
#include <Arduino.h>

#include <gfx/OLED.hpp>


gfx::OLED display;

void test_demo() {
    display.setFont(gfx::Font::DOUBLE_THIN);
    display.print("TEST");

    TEST_ASSERT(true);
}

void setup() {
    display.init();

    UNITY_BEGIN();
    RUN_TEST(test_demo);
    UNITY_END();
}

void loop() {

}
