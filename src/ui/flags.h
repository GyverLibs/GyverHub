#pragma once
#include <Arduino.h>

namespace gh {

struct Flags {
    uint16_t flags = 0;

    Flags() {}
    Flags(const Flags& f) = default;
    Flags(uint16_t nflags) {
        flags = nflags;
    }

    void clear(uint8_t idx) {
        write(idx, 0);
    }
    void set(uint8_t idx) {
        write(idx, 1);
    }
    void write(uint8_t idx, uint8_t val) {
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

}