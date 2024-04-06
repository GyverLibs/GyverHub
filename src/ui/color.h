#pragma once
#include <Arduino.h>

#include "core/types.h"

namespace gh {

// цвет
enum class Colors : uint32_t {
    Red = 0xcb2839,
    Orange = 0xd55f30,
    Yellow = 0xd69d27,
    Green = 0x37A93C,
    Mint = 0x25b18f,
    Aqua = 0x2ba1cd,
    Blue = 0x297bcd,
    Violet = 0x825ae7,
    Pink = 0xc8589a,
    Default = 0xffffffff,
};

struct Color {
    uint8_t r = 0, g = 0, b = 0;

    Color() {}
    Color(const Color& col) = default;
    Color(Colors color) {
        setHEX((uint32_t)color);
        if (color == Colors::Default) _default = 1;
    }
    Color(uint32_t hex, bool gray = false) {
        if (gray) setGray(gray);
        else setHEX(hex);
    }
    Color(uint8_t v1, uint8_t v2, uint8_t v3, bool hsv = false) {
        if (hsv) setHSV(v1, v2, v3);
        else setRGB(v1, v2, v3);
    }

    // static
    static Color fromRGB(uint8_t r, uint8_t g, uint8_t b) {
        return Color(r, g, b, false);
    }
    static Color fromHSV(uint8_t h, uint8_t s, uint8_t v) {
        return Color(h, s, v, true);
    }
    static Color fromHEX(uint32_t hex) {
        return Color(hex, false);
    }
    static Color fromGray(uint8_t gray) {
        return Color(gray, true);
    }
    static Color fromHue(uint8_t hue) {
        Color col;
        col.setHue(hue);
        return col;
    }
    static Color from565(uint16_t rgb565) {
        Color col;
        col.set565(rgb565);
        return col;
    }
    static Color fromColors(Colors color) {
        return Color(color);
    }

    // установить RGB цвет
    void setRGB(uint8_t nr, uint8_t ng, uint8_t nb) {
        r = nr;
        g = ng;
        b = nb;
    }

    // установить серый цвет 0.. 255
    void setGray(uint8_t gray) {
        setRGB(gray, gray, gray);
    }

    // установить цвет 24-бит RGB
    void setHEX(uint32_t hex) {
        r = ((uint8_t*)&hex)[2];
        g = ((uint8_t*)&hex)[1];
        b = ((uint8_t*)&hex)[0];
    }

    // установить цвет 16-бит RGB
    void set565(uint16_t col) {
        r = (col & 0b1111100000000000) >> 8;
        g = (col & 0b0000011111100000) >> 3;
        b = (col & 0b0000000000011111) << 3;
    }

    // установить цвет HSV
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

    // установить цвет по радуге (0.. 255)
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

    // получить 24-бит RGB цвет
    uint32_t getHEX() const {
        // uint8_t rgb[] = {b, g, r, 0};
        // return *((uint32_t*)&rgb[0]);
        union RGB {
            uint8_t bytes[4];
            uint32_t hex;
        };
        return RGB{b, g, r, 0}.hex;
    }

    // получить 16-бит RGB цвет
    uint16_t get565() const {
        return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | ((b & 0b11111000) >> 3);
    }

    operator uint32_t() const {
        return getHEX();
    }

    bool isDefault() const {
        return _default;
    }

   private:
    bool _default = 0;
};

}  // namespace gh