#pragma once
#include <Arduino.h>

namespace gh {

// расстояние между двумя точками
uint16_t __attribute__((weak)) dist(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    x1 -= x0;
    y1 -= y0;
    return sqrt(x1 * x1 + y1 * y1);
}

// точка лежит внутри прямоугольника (координаты угла и размеры)
bool __attribute__((weak)) inRect(int16_t x, int16_t y, int16_t rx, int16_t ry, uint16_t w, uint16_t h) {
    return (x >= rx) && (x < int16_t(rx + w)) && (y >= ry) && (y < int16_t(ry + h));
}

// точка лежит внутри окружности (координаты центра и радиус)
bool __attribute__((weak)) inCircle(int16_t x, int16_t y, int16_t cx, int16_t cy, uint16_t r) {
    cx -= x;
    cy -= y;
    return (uint16_t(cx * cx) + uint16_t(cy * cy) <= r * r);
}

}