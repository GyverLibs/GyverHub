#pragma once
#include <Arduino.h>

#include "../config.hpp"

enum GHmodule_t {
    GH_MOD_INFO = (1ul << 0),
    GH_MOD_FSBR = (1ul << 1),
    GH_MOD_FORMAT = (1ul << 2),
    GH_MOD_DOWNLOAD = (1ul << 3),
    GH_MOD_UPLOAD = (1ul << 4),
    GH_MOD_OTA = (1ul << 5),
    GH_MOD_OTA_URL = (1ul << 6),
    GH_MOD_REBOOT = (1ul << 7),
    GH_MOD_SET = (1ul << 8),
    GH_MOD_READ = (1ul << 9),
    GH_MOD_DELETE = (1ul << 10),
    GH_MOD_RENAME = (1ul << 11),
    GH_MOD_SERIAL = (1ul << 12),
    GH_MOD_BT = (1ul << 13),
    GH_MOD_WS = (1ul << 14),
    GH_MOD_MQTT = (1ul << 15),
};

struct GHmodule {
    uint16_t mods = 0;

    void set(uint16_t nmods) {
        mods &= ~nmods;
    }
    void unset(uint16_t nmods) {
        mods |= nmods;
    }

    void setAll() {
        mods = 0;
    }
    void unsetAll() {
        mods = (1ul << 16) - 1;
    }
    bool read(GHmodule_t m) {
        return !(mods & m);
    }
};