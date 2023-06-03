#pragma once
#include <Arduino.h>

class GHpos {
   public:
    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }
    virtual int16_t x() {
        return _x;
    }
    virtual int16_t y() {
        return _y;
    }

    int16_t _x = 0;
    int16_t _y = 0;
    bool _changed = 0;

   private:
};