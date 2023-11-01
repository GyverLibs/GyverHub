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
    GHcolor(uint8_t gray) {
        setGray(gray);
    }
    GHcolor(uint32_t hex, bool f) {
        if (f) setHEX(hex);
    }
    GHcolor(uint8_t v1, uint8_t v2, uint8_t v3, bool hsv = 0) {
        if (hsv) setHSV(v1, v2, v3);
        else setRGB(v1, v2, v3);
    }

    void setRGB(uint8_t nr, uint8_t ng, uint8_t nb) {
        r = nr;
        g = ng;
        b = nb;
    }
    void setGray(uint8_t gray) {
        setRGB(gray, gray, gray);
    }
    void setHEX(uint32_t hex) {
        r = ((uint8_t*)&hex)[2];
        g = ((uint8_t*)&hex)[1];
        b = ((uint8_t*)&hex)[0];
    }
    void setHSV(uint8_t h, uint8_t s, uint8_t v) {
        float R, G, B;

        float H = h / 255.0;
        float S = s / 255.0;
        float V = v / 255.0;

        uint8_t i = H * 6;
        float f = H * 6 - i;
        float p = V * (1 - S);
        float q = V * (1 - f * S);
        float t = V * (1 - (1 - f) * S);

        switch (i) {
            case 0:
                R = V, G = t, B = p;
                break;
            case 1:
                R = q, G = V, B = p;
                break;
            case 2:
                R = p, G = V, B = t;
                break;
            case 3:
                R = p, G = q, B = V;
                break;
            case 4:
                R = t, G = p, B = V;
                break;
            case 5:
                R = V, G = p, B = q;
                break;
        }
        r = R * 255;
        g = G * 255;
        b = B * 255;
    }
    void setHue(uint8_t color) {
        uint8_t shift;
        if (color > 170) {
            shift = (color - 170) * 3;
            r = shift;
            g = 0;
            b = 255 - shift;
        } else if (color > 85) {
            shift = (color - 85) * 3;
            r = 0;
            g = 255 - shift;
            b = shift;
        } else {
            shift = color * 3;
            r = 255 - shift;
            g = shift;
            b = 0;
        }
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