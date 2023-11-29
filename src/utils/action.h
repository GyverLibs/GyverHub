#pragma once
#include <Arduino.h>

#include "color.h"
#include "flags.h"
#include "pos.h"

struct GHaction {
    // имя компонента
    const char* name = nullptr;

    // значение компонента
    const char* value = nullptr;

    // value 
    // получить значение как int (32 бит)
    int32_t valueInt() const {
        return atol(value);
    }

    // получить значение как float
    float valueFloat() const {
        return atof(value);
    }

    // получить значение как const char*
    const char* valueStr() const {
        return value;
    }

    // получить значение как bool
    bool valueBool() const {
        return (value[0] == '1');
    }

    // получить значение как String
    String valueString() const {
        return value;
    }

    // получить значение как GHcolor
    GHcolor valueColor() const {
        return GHcolor(atol(value), HEX);
    }

    // получить значение как GHflags
    GHflags valueFlags() const {
        return GHflags(atoi(value));
    }

    // получить значение как GHpos
    GHpos valuePos() const {
        uint32_t xy = atol(value);
        return GHpos(xy >> 16, xy & 0xffff, 1);
    }

    // name
    // получить имя как String
    String nameString() const {
        return name;
    }

    // получить имя как const char*
    const char* nameStr() const {
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