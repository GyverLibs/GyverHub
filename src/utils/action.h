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
};