#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/fs.h"
#include "hub_macro.hpp"

/*
    Модуль CSV таблицы с хранением в файле
    - Не даёт записать больше столбцов, чем указано
    - Добивает незаполненные столбцы нулями при завершении строки
    - Следит за количеством строк (если задано) и автоматически сокращает файл
    - После ручного редактирования файла нужно вызвать update
*/

namespace gh {

#if defined(GH_ESP_BUILD) && !defined(GH_NO_FS)
class CSVFile {
   public:
    // создать таблицу (путь, столбцов, строк)
    CSVFile(const char* path, uint8_t cols, uint16_t rows = 0) : _path(path), _cols(cols), _rows(rows) {}

    // добавить int/string/bool
    CSVFile& add(const su::Value& value) {
        if (_col < _cols && _opened()) {
            _checkFirst();
            _file.print(value);
            _col++;
        }
        return *this;
    }

    // добавить float
    CSVFile& add(double value, uint8_t dec) {
        return add(su::Value(value, dec));
    }

    // завершить строку
    void endRow(bool close = true) {
        if (_opened()) {
            while (_col < _cols) {
                _file.print(";0");
                _col++;
            }
            _file.println();
            _first = true;
            _row++;
            _col = 0;

            if (_rows && _row > _rows) {
                _file.close();
                _file = FS.openRead(_path);
                if (_file) {
                    uint16_t row = _row;
                    while (_file.available() && row > _rows) {
                        if (_file.read() == '\n') row--;
                    }
                    String npath(_path);
                    npath += F(".new");
                    File file = FS.openWrite(npath);
                    if (file) {
                        file.write(_file);
                        file.close();
                        _file.close();
                        if (FS.remove(_path, false) && FS.rename(npath, _path)) {
                            _row = row;
                        }
                    }
                }
            }
            if (!close) _opened();
        }
    }

    // получить значение ячейки (столбец, строка)
    String get(uint8_t col, uint16_t row) {
        String s;
        if (col >= _cols || row >= _rows) return s;
        if (_file) _file.close();
        _file = FS.openRead(_path);
        if (!_file) return s;

        char endc = (col == _cols - 1) ? '\n' : ';';
        while (_file.available()) {
            char ch = _file.read();
            if (row) {
                if (ch == '\n') row--;
            } else {
                if (col) {
                    if (ch == ';') col--;
                } else {
                    if (ch == endc) break;
                    s += ch;
                }
            }
        }
        _file.close();
        return s;
    }

    // очистить таблицу
    void clear() {
        _col = 0;
        _row = 0;
        if (_file) _file.close();
        _file = FS.openWrite(_path);
        if (_file) _file.close();
    }

    // пересчитать количество строк (после ручного изменения)
    void update() {
        _row = 0;
        if (_file) _file.close();
        _file = FS.openRead(_path);
        while (_file.available()) {
            if (_file.read() == '\n') _row++;
        }
        if (_file) _file.close();
        _updated = true;
    }

   private:
    const char* _path = nullptr;
    uint8_t _cols, _col = 0;
    uint16_t _rows, _row = 0;
    File _file;
    bool _first = true;
    bool _updated = false;

    bool _opened() {
        if (!_updated) update();
        if (!_file) _file = FS.openAdd(_path);
        return _file;
    }
    void _checkFirst() {
        if (_first) _first = false;
        else _file.print(';');
    }
};
#endif

}  // namespace gh