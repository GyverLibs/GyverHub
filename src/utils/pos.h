#pragma once
#include <Arduino.h>
#include "pos_geo.h"

class GHpos {
   public:
    GHpos() {}
    GHpos(int16_t nx, int16_t ny, bool nc = 0) : x(nx), y(ny), _changed(nc) {}

    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }

    // расстояние до точки
    int16_t dist(int16_t x1, int16_t y1) {
        return GHdist(x, y, x1, y1);
    }

    // точка лежит внутри прямоугольника
    bool inRect(int16_t rx, int16_t ry, int16_t w, int16_t h) {
        return GHinRect(x, y, rx, ry, w, h);
    }

    // точка лежит внутри окружности
    bool inCircle(int16_t cx, int16_t cy, int16_t r) {
        return GHinCircle(x, y, cx, cy, r);
    }

    // координаты
    int16_t x = 0;
    int16_t y = 0;

    bool _changed = 0;
};