#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "flag.h"
#include "pos_func.h"

namespace gh {

class Pos : public Flag {
   public:
    Pos() {}
    Pos(const su::Text& text, bool change = 0) : Flag(change) {
        int16_t* xy[] = {&x, &y};
        text.split(xy, 2, ';');
    }
    Pos(int16_t nx, int16_t ny, bool change = 0) : Flag(change), x(nx), y(ny) {}

    // bool changed();

    // расстояние до точки
    uint16_t dist(int16_t x1, int16_t y1) {
        return gh::dist(x, y, x1, y1);
    }

    // точка лежит внутри прямоугольника
    bool inRect(int16_t rx, int16_t ry, uint16_t w, uint16_t h) {
        return gh::inRect(x, y, rx, ry, w, h);
    }

    // точка лежит внутри окружности
    bool inCircle(int16_t cx, int16_t cy, uint16_t r) {
        return gh::inCircle(x, y, cx, cy, r);
    }

    // координата x
    int16_t x = 0;

    // координата y
    int16_t y = 0;
};

}  // namespace gh