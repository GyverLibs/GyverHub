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

    // private
    bool nameEq(VSPTR cname, bool fstr) {
        if (cname) return fstr ? !strcmp_P(name, (PGM_P)cname) : !strcmp(name, (PGM_P)cname);
        else return autoNameEq();
    }
    bool autoNameEq() {
        return name[0] == '_' && name[1] == 'n' && (uint16_t)atoi(name + 2) == count;
    }

    uint16_t count = 0;
};