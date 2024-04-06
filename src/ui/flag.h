#pragma once
#include <Arduino.h>

#include "widget_class.h"

namespace gh {

class Flag {
    friend class ghc::Widget;

   public:
    Flag(bool change = false) : _changed(change) {}

    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }

    void clear() {
        _changed = 0;
    }

    operator bool() {
        return changed();
    }

    void _change() {
        _changed = 1;
    }

   protected:
    bool _changed = 0;
};

}  // namespace gh