#pragma once
#include <Arduino.h>

// тип действия
enum GHaction_t {
    GH_ACTION_NONE,
    GH_ACTION_SET,
    GH_ACTION_PRESS,
    GH_ACTION_RELEASE,
};

struct GHaction {
    // тип действия
    GHaction_t type = GH_ACTION_NONE;   

    // имя компонента
    const char* name = nullptr;

    // значение компонента
    const char* value = nullptr;

    // получить значение как int (32 бит)
    int32_t valueInt() const {
        return atol(value);
    }

    // получить значение как float
    float valueFloat() const {
        return atof(value);
    }

    // получить значение как String
    String valueString() const {
        return value;
    }

    // получить имя как String
    String nameString() const {
        return name;
    }
};