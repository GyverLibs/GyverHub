#pragma once
#include <Arduino.h>

#include "config.h"

enum GHmodule_t {
    GH_MOD_MANUAL = 0b1,
    GH_MOD_STREAM = 0b10,
    GH_MOD_LOCAL = 0b100,
    GH_MOD_MQTT = 0b1000,
    GH_MOD_INFO = 0b10000,
    GH_MOD_FSBR = 0b100000,
    GH_MOD_DOWNLOAD = 0b1000000,
    GH_MOD_UPLOAD = 0b10000000,
    GH_MOD_OTA = 0b100000000,
    GH_MOD_OTA_URL = 0b1000000000,
    GH_MOD_REBOOT = 0b10000000000,
    GH_MOD_SET = 0b100000000000,
    GH_MOD_CLICK = 0b1000000000000,
    GH_MOD_READ = 0b10000000000000,
    GH_MOD_DELETE = 0b100000000000000,
    GH_MOD_RENAME = 0b1000000000000000,
};

struct GHmodule {
    uint16_t mods = 0xffff;

    void set(uint16_t nmods) {
        mods |= nmods;
    }
    void unset(uint16_t nmods) {
        mods &= ~nmods;
    }

    void setAll() {
        mods = 0xffff;
    }
    void unsetAll() {
        mods = 0;
    }
    bool read(GHmodule_t m) {
        return mods & m;
    }
};