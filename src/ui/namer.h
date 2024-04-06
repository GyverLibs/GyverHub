#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"

namespace ghc {

class Namer {
   public:
    Namer(GHTREF name) : _buildName(name) {
        if (name.length() >= 2 && name[0] == '_' && name[1] == 'n') {
            _buildIndex = name.substring(2).toInt16();
        }
    }

    // если имя не задано - увеличивает индекс
    void check(GHTREF name = GHTXT()) {
        if (!name.length()) _index++;
    }

    // проверяет совпадение для заданного имени и индекс для не заданного
    bool equals(GHTREF name) {
        if (!_found && (name.valid() ? (_buildName == name) : (_buildIndex == _index))) {
            _found = true;
            return 1;
        }
        return 0;
    }

    // добавляет индекс к строке
    void addIndex(String& s) {
        s += F("_n");
        s += _index;
    }

    // виджет с нужным именем был найден
    bool isFound() {
        return _found;
    }

   private:
    GHTREF _buildName;
    int16_t _index = 0;
    int16_t _buildIndex = -1;
    bool _found = 0;
};

}  // namespace ghc