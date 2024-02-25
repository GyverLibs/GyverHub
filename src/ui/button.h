#pragma once
#include <Arduino.h>

namespace gh {

class Button {
   public:
    // состояние кнопки изменилось
    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }

    // состояние кнопки (1 нажата, 0 отпущена)
    bool state() {
        return _state;
    }

    // клик по кнопке
    bool clicked() {
        return _clicked ? (_clicked = 0, 1) : 0;
    }

    //    private:
    bool _state = 0;
    bool _changed = 0;
    bool _clicked = 0;
};

}  // namespace gh