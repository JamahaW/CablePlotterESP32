#pragma once

#define MAKE_FONT(width_mul, height_mul) char(((width_mul) & 0xF) | ((height_mul) << 4))


namespace gfx {
    /// @brief Маркер шрифта
    enum class Font : unsigned char {
        SINGLE = MAKE_FONT(1, 1),
        SINGLE_WIDE = MAKE_FONT(2, 1),
        DOUBLE_THIN = MAKE_FONT(1, 2),
        DOUBLE = MAKE_FONT(2, 2),
        DOUBLE_WIDE = MAKE_FONT(3, 2),
    };
}