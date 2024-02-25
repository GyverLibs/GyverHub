#pragma once
#include <Arduino.h>

#include "builder.h"

namespace gh {

class Show {
   public:
    Show(Builder& b, bool en) : b(b) {
        b.show(en);
    }
    ~Show() {
        b.show(true);
    }

   private:
    Builder& b;
};

class Row {
   public:
    Row(Builder& b, uint16_t width = 1) : b(b) {
        b.beginRow(width);
    }
    ~Row() {
        b.endRow();
    }

   private:
    Builder& b;
};

class Col {
   public:
    Col(Builder& b, uint16_t width = 1) : b(b) {
        b.beginCol(width);
    }
    ~Col() {
        b.endCol();
    }

   private:
    Builder& b;
};

}  // namespace gh