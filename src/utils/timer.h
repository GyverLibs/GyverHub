#pragma once
#include <Arduino.h>

// ====================== TIMER =======================
struct GHtimer {
    GHtimer() {}
    GHtimer(uint32_t ms, uint8_t seconds = 0, uint8_t minutes = 0, uint8_t hours = 0, uint8_t days = 0) {
        start(ms, seconds, minutes, hours, days);
    }

    void start(uint32_t ms, uint8_t seconds = 0, uint8_t minutes = 0, uint8_t hours = 0, uint8_t days = 0) {
        prd = days * 86400ul + hours * 3600ul + minutes * 60 + seconds;
        prd *= 1000ul;
        prd += ms;
        if (prd) start();
    }
    void start() {
        if (!prd) return;
        tmr = millis();
        if (!tmr) tmr = 1;
    }
    void stop() {
        tmr = 0;
    }
    bool ready() {
        return (tmr && millis() - tmr >= prd) ? (start(), true) : false;
    }
    operator bool() {
        return ready();
    }

    uint32_t tmr = 0, prd = 0;
};
