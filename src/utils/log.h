#pragma once
#include <Print.h>

#include "misc.h"

class GHlog : public Print {
   public:

    void begin(int n = 64) {
        end();
        len = head = 0;
        size = n;
        buffer = new char[size];
    }

    ~GHlog() {
        end();
    }

    void end() {
        if (buffer) {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    virtual size_t write(uint8_t n) {
        if (buffer) _write(n);
        return 1;
    }

    void read(String* s) {
        if (!buffer) return;
        bool start = 0;
        for (uint16_t i = 0; i < len; i++) {
            char c = _read(i);
            if (start && c != '\r') GH_escapeChar(s, c);
            else if (c == '\n') start = 1;
        }
    }

    String read() {
        String s;
        s.reserve(len);
        read(&s);
        return s;
    }

    void clear() {
        len = head = 0;
    }

    bool available() {
        return (buffer && len);
    }

    bool state() {
        return buffer;
    }

    int length() {
        return len;
    }

    char* buffer = nullptr;

   private:
    void _write(uint8_t n) {
        if (len < size) len++;
        buffer[head] = n;
        if (++head >= size) head = 0;
    }
    char _read(int num) {
        return buffer[(len < size) ? num : ((head + num) % size)];
    }

    uint16_t size = 0;
    uint16_t len = 0;
    uint16_t head = 0;
};