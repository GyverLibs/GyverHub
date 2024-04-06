#pragma once
#include <Arduino.h>

#include "flag.h"

namespace gh {

class Button : public Flag {
   public:
    // клик по кнопке
    bool click() {
        return _click ? (_click = 0, 1) : 0;
    }

    // состояние кнопки (1 нажата, 0 отпущена)
    bool state() {
        return _state;
    }

    // состояние кнопки изменилось
    // bool changed();

    bool _state = 0;
    bool _click = 0;
};

}  // namespace gh