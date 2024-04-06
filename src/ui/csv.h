#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"

/*
    Модуль CSV таблицы с хранением в строке
    - Не даёт записать больше столбцов, чем указано
    - Добивает незаполненные столбцы нулями при завершении строки
    - Следит за количеством строк (если задано) и автоматически сокращает строку
    - После ручного редактирования строки нужно вызвать update
*/

namespace gh {

class CSV {
   public:
    // создать таблицу (столбцов, строк)
    CSV(uint8_t cols, uint16_t rows = 0) : _cols(cols), _rows(rows) {}

    // зарезервировать строку
    void reserve(uint32_t res) {
        str.reserve(res);
    }

    // добавить int/string/bool
    CSV& add(const su::Value& value) {
        if (_col < _cols) {
            _checkFirst();
            value.addString(str);
            _col++;
        }
        return *this;
    }

    // добавить float
    CSV& add(double value, uint8_t dec) {
        return add(su::Value(value, dec));
    }

    // завершить строку
    void endRow() {
        while (_col < _cols) {
            str += ";0";
            _col++;
        }
        str += '\n';
        _first = true;
        _row++;
        _col = 0;
        if (_rows && _row > _rows) {
            int16_t p = 0;
            while (_row > _rows) {
                p = str.indexOf('\n', p + 1);
                _row--;
            }
            if (p > 0) str.remove(0, p + 1);
        }
    }

    // получить значение ячейки (столбец, строка)
    String get(uint8_t col, uint16_t row) {
        if (col >= _cols || row >= _rows) return String();
        char endc = (col == _cols - 1) ? '\n' : ';';
        int16_t p = 0;
        while (row--) {
            p = str.indexOf('\n', p + 1);
            if (p < 0) return String();
        }
        while (col--) {
            p = str.indexOf(';', p + 1);
            if (p < 0) return String();
        }
        int16_t d = str.indexOf(endc, p + 1);
        if (d < 0) return String();
        if (p) p++;
        return str.substring(p, d);
    }

    // очистить таблицу
    void clear() {
        str = "";
        _col = 0;
        _row = 0;
    }

    // пересчитать количество строк (после ручного изменения)
    void update() {
        _row = 0;
        int16_t p = 0;
        while (true) {
            p = str.indexOf('\n', p + 1);
            if (p < 0) break;
            _row++;
        }
    }

    operator String&() {
        return str;
    }

    String str;

   private:
    bool _first = true;
    uint8_t _cols, _col = 0;
    uint16_t _rows, _row = 0;

    void _checkFirst() {
        if (_first) _first = false;
        else str += ';';
    }
};

}  // namespace gh