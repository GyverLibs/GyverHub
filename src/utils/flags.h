#pragma once
#include <Arduino.h>

struct GHflags {
    uint16_t flags = 0;

    GHflags() {}
    GHflags(const GHflags& f) = default;
    GHflags(uint16_t nflags) {
        flags = nflags;
    }

    void set(uint8_t idx, uint8_t val) {
        if (idx < 16) bitWrite(flags, idx, val);
    }
    uint8_t get(uint8_t idx) {
        if (idx < 16) return bitRead(flags, idx);
        else return 0;
    }

    String toString() {
        String s;
        s.reserve(16);
        for (int i = 0; i < 16; i++) s += get(i);
        return s;
    }
};