#pragma once
#include <Print.h>

#include "misc.h"

class GHlog : public Print {
   public:
    // начать и указать размер буфера
    void begin(int n = 64) {
        end();
        size = n;
        buffer = new char[size];
        clear();
    }

    ~GHlog() {
        end();
    }

    // остановить
    void end() {
        if (buffer) {
            delete[] buffer;
            buffer = nullptr;
        }
    }

    virtual size_t write(uint8_t n) {
        if (buffer) {
            _write(n);
        }
        return 1;
    }

    // прочитать в строку
    void read(String* s, bool esc = false) {
        if (!buffer) return;
        bool start = 0;
        for (uint16_t i = 0; i < len; i++) {
            char c = _read(i);
            if (start) {
                if (esc) {
                    if (c == '\"') *s += '\\';
                }
                *s += c;
            } else if (c == '\n') start = 1;
        }
    }

    // прочитать строкой
    String read() {
        String s;
        s.reserve(len);
        read(&s);
        return s;
    }

    // очистить
    void clear() {
        len = head = 0;
        _write('\n');
    }

    // есть данные
    bool available() {
        return (buffer && len);
    }

    // запущен
    bool state() {
        return buffer;
    }

    // длина
    int length() {
        return len;
    }

    char* buffer = nullptr;

   private:
    void _write(uint8_t n) {
        if (n == '\r') return;
        if (len < size) len++;
        buffer[head] = n;
        if (++head >= size) head = 0;
    }
    char _read(uint16_t num) {
        return buffer[(len < size) ? num : ((head + num) % size)];
    }

    uint16_t size = 0;
    uint16_t len = 0;
    uint16_t head = 0;
};