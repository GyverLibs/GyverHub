#pragma once
#include <Arduino.h>

#include "widget_class.h"

namespace gh {

class Flag {
    friend class ghc::Widget;

   public:
    bool changed() {
        return _changed ? (_changed = 0, 1) : 0;
    }

    operator bool() {
        return changed();
    }

   private:
    bool _changed = 0;
};

}  // namespace gh