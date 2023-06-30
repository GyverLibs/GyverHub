#pragma once
#include <Arduino.h>

#include "../config.hpp"
#include "../macro.hpp"
#include "action.h"
#include "datatypes.h"
#include "client.h"
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
    GHbuild(GHbuild_t btype = GH_BUILD_NONE, const char* name = nullptr, const char* value = nullptr, GHclient nclient = GHclient(), GHevent_t ncmd = GH_IDLE) {
        type = btype;
        action.name = name;
        action.value = value;
        client = nclient;
        cmd = ncmd;
    }

    bool parse(VSPTR cname, void* var, GHdata_t dtype, bool fstr) {
        if (type == GH_BUILD_ACTION && action.nameEq(cname, fstr)) {
            type = GH_BUILD_NONE;
            GHtypeFromStr(action.value, var, dtype);
            return 1;
        }
        return 0;
    }

    // тип билда
    GHbuild_t type = GH_BUILD_NONE;

    // данные клиента
    GHclient client;

    // действие
    GHaction action;

    // событие
    GHevent_t cmd = GH_IDLE;
};