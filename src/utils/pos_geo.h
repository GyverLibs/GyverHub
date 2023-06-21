#pragma once
#include <Arduino.h>

// расстояние между двумя точками
int16_t GHdist(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

// точка лежит внутри прямоугольника (координаты угла и размеры)
bool GHinRect(int16_t x, int16_t y, int16_t rx, int16_t ry, int16_t w, int16_t h);

// точка лежит внутри окружности (координаты центра и радиус)
bool GHinCircle(int16_t x, int16_t y, int16_t cx, int16_t cy, int16_t r);