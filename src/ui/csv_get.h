#pragma once
#include <Arduino.h>

#include "core/fs.h"
#include "hub_macro.hpp"

namespace gh {

String __attribute__((weak)) getCSV(String& str, uint16_t col, uint16_t row, char div) {
    String buf;
    bool quote = false;
    bool found = false;
    uint16_t trow = 0, tcol = 0;
    for (uint32_t c = 0; c < str.length(); c++) {
        char cc = str[c], nc = str[c + 1];

        if (trow == row && tcol == col) {
            if (!found) found = true;
        } else {
            if (found) break;
        }
        if (quote && cc == '\"' && nc == '\"') {
            if (found) buf += cc;
            c++;
            continue;
        }
        if (cc == '\"') {
            quote = !quote;
            continue;
        }
        if (!quote && cc == div) {
            tcol++;
            continue;
        }
        if (!quote && cc == '\r' && nc == '\n') {
            tcol = 0;
            trow++;
            c++;
            continue;
        }
        if (!quote && (cc == '\r' || cc == '\n')) {
            trow++;
            tcol = 0;
            continue;
        }
        if (found) buf += cc;
    }
    return buf;
}

#if defined(GH_ESP_BUILD) && !defined(GH_NO_FS)
String __attribute__((weak)) getCSV(File& csv, uint16_t col, uint16_t row, char div) {
    String buf;
    bool quote = false;
    bool found = false;
    uint16_t trow = 0, tcol = 0;
    while (csv.available()) {
        char cc = csv.read();
        char nc = csv.peek();

        if (trow == row && tcol == col) {
            if (!found) found = true;
        } else {
            if (found) break;
        }
        if (quote && cc == '\"' && nc == '\"') {
            if (found) buf += cc;
            csv.read();
            continue;
        }
        if (cc == '\"') {
            quote = !quote;
            continue;
        }
        if (!quote && cc == div) {
            tcol++;
            continue;
        }
        if (!quote && cc == '\r' && nc == '\n') {
            tcol = 0;
            trow++;
            csv.read();
            continue;
        }
        if (!quote && (cc == '\r' || cc == '\n')) {
            trow++;
            tcol = 0;
            continue;
        }
        if (found) buf += cc;
    }
    return buf;
}
#endif

}  // namespace gh