#pragma once
#include <Arduino.h>

#include "pos_func.h"

namespace gh {

class Pos {
   public:
    Pos() {}
    Pos(int16_t nx, int16_t ny, bool nc = 0) : x(nx), y(ny), _changed(nc) {}

    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }

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

    //    private:
    bool _changed = 0;
};

}  // namespace gh