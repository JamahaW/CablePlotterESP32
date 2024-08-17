#include "tools/SDTool.hpp"

const char *tools::sd::readableType(sdcard_type_t type) {
    constexpr static const char *type_strings[]{
            "None",     // CARD_NONE
            "MMC",      // CARD_MMC
            "SD",       // CARD_SD
            "SDHC",     // CARD_SDHC
            "Unknown",  // CARD_UNKNOWN
    };
    return type_strings[type];
}

