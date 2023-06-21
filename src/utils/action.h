#pragma once
#include <Arduino.h>


struct GHaction {
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

    bool flag = 0;
};