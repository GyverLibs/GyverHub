#pragma once
#include <Arduino.h>

// ==================== COLORS =====================
enum GHcolors {
    GH_RED = 0xcb2839,
    GH_ORANGE = 0xd55f30,
    GH_YELLOW = 0xd69d27,
    GH_GREEN = 0x37A93C,
    GH_MINT = 0x25b18f,
    GH_AQUA = 0x2ba1cd,
    GH_BLUE = 0x297bcd,
    GH_VIOLET = 0x825ae7,
    GH_PINK = 0xc8589a,
    GH_DEFAULT = 0xffffffff,
};

// ====================== COLOR =======================
struct GHcolor {
    uint8_t r = 0, g = 0, b = 0;

    GHcolor() {}
    GHcolor(const GHcolor& col) = default;
    GHcolor(uint32_t col) {
        setHEX(col);
    }
    GHcolor(uint8_t nr, uint8_t ng, uint8_t nb) {
        setRGB(nr, ng, nb);
    }

    void setRGB(uint8_t nr, uint8_t ng, uint8_t nb) {
        r = nr;
        g = ng;
        b = nb;
    }
    void setHEX(uint32_t hex) {
        r = ((uint8_t*)&hex)[2];
        g = ((uint8_t*)&hex)[1];
        b = ((uint8_t*)&hex)[0];
    }
    uint32_t getHEX() {
        uint32_t hex = 0;
        ((uint8_t*)&hex)[2] = r;
        ((uint8_t*)&hex)[1] = g;
        ((uint8_t*)&hex)[0] = b;
        return hex;
    }
    operator uint32_t() {
        return getHEX();
    }
};