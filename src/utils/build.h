#pragma once
#include <Arduino.h>

#include "../config.hpp"
#include "../macro.hpp"
#include "action.h"
#include "datatypes.h"
#include "hub.h"
#include "stats.h"

// тип билда
enum GHbuild_t {
    GH_BUILD_NONE,
    GH_BUILD_ACTION,
    GH_BUILD_COUNT,
    GH_BUILD_READ,
    GH_BUILD_UI,
    GH_BUILD_TG,
};

class GHbuild {
   public:
    GHbuild(GHbuild_t btype = GH_BUILD_NONE, bool act = 0, const char* name = nullptr, const char* value = nullptr, GHhub nhub = GHhub()) {
        type = btype;
        action.flag = act;
        action.name = name;
        action.value = value;
        hub = nhub;
    }

    bool nameEq(VSPTR name, bool fstr) {
        if (name) return fstr ? !strcmp_P(action.name, (PGM_P)name) : !strcmp(action.name, (PGM_P)name);
        else return autoNameEq();
    }

    bool parse(VSPTR name, void* var, GHdata_t type, bool fstr) {
        if (action.flag && nameEq(name, fstr)) {
            action.flag = 0;
            GHtypeFromStr(action.value, var, type);
            return 1;
        }
        return 0;
    }

    bool autoNameEq() {
        return action.name[0] == '_' && action.name[1] == 'n' && atoi(action.name + 2) == count;
    }

    // тип билда
    GHbuild_t type = GH_BUILD_NONE;

    // данные клиента
    GHhub hub;

    // действие
    GHaction action;

    // private
    uint16_t count = 0;
};