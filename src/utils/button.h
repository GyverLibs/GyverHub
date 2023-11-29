#pragma once
#include <Arduino.h>

class GHbutton {
   public:
    bool state = 0;

    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }

    operator bool() {
        return state;
    }

    bool _changed = 0;
};