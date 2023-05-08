#pragma once
#include <Arduino.h>

// ====================== TIMER =======================
struct GHtimer {
    GHtimer() {}
    GHtimer(uint32_t nprd) {
        start(nprd);
    }

    void start(uint32_t nprd) {
        prd = nprd;
        if (prd) start();
    }
    void start(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days) {
        prd = days * 86400ul + hours * 3600ul + minutes * 60 + seconds;
        prd *= 1000ul;
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

    uint32_t tmr = 0, prd = 0;
};
