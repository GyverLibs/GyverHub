#pragma once
#include <Arduino.h>

#include "config.hpp"
#include "action.h"
#include "datatypes.h"
#include "hub.h"
#include "stats.h"
#include "macro.hpp"

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
    GHbuild(GHbuild_t btype = GH_BUILD_NONE, GHaction_t atype = GH_ACTION_NONE, const char* name = nullptr, const char* value = nullptr, GHhub nhub = GHhub()) {
        type = btype;
        action.type = atype;
        action.name = name;
        action.value = value;
        hub = nhub;
    }

    // нажата кнопка с именем name. fstr - имя передано как F() или PSTR()
    bool press(VSPTR name, bool fstr = true) {
        return check(GH_ACTION_PRESS, name, fstr);
    }

    // отпущена кнопка с именем name. fstr - имя передано как F() или PSTR()
    bool release(VSPTR name, bool fstr = true) {
        return check(GH_ACTION_RELEASE, name, fstr);
    }

    // установка значения на компонент с именем name. fstr - имя передано как F() или PSTR()
    bool set(VSPTR name, bool fstr = true) {
        return check(GH_ACTION_SET, name, fstr);
    }

    // имя действия совпадает с именем name. fstr - имя передано как F() или PSTR()
    bool nameEq(VSPTR name, bool fstr = true) {
        return fstr ? !strcmp_P(action.name, (PGM_P)name) : !strcmp(action.name, (PGM_P)name);
    }

    // парсить значение компонента с именем name. fstr - имя передано как F() или PSTR()
    bool parseSet(VSPTR name, void* value, GHdata_t type, bool fstr = true) {
        if (set(name, fstr)) {
            GHtypeFromStr(action.value, value, type);
            return 1;
        } else return 0;
    }

    // парсить действие по кнопке с именем name. fstr - имя передано как F() или PSTR()
    bool parseClick(VSPTR name, bool* value, bool fstr = true) {
        if (press(name, fstr)) {
            if (value) *value = 1;
            return 1;
        } else if (release(name, fstr)) {
            if (value) *value = 0;
            return 0;
        }
        return 0;
    }

    // тип билда
    GHbuild_t type = GH_BUILD_NONE;

    // данные клиента
    GHhub hub;

    // действие
    GHaction action;

   private:
    bool check(GHaction_t atype, VSPTR name, bool fstr) {
        if (action.type == atype && nameEq(name, fstr)) {
            action.type = GH_ACTION_NONE;
            return 1;
        }
        return 0;
    }
};