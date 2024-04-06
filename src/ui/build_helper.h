#pragma once
#include <Arduino.h>

#include "builder.h"
#include "builder_macro.h"

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

// строка
GH_BUILD_CONTAINER_CLASS(Row);

// столбец
GH_BUILD_CONTAINER_CLASS(Col);

// спойлер строка
GH_BUILD_CONTAINER_CLASS(SpoilerRow);

// спойлер столбец
GH_BUILD_CONTAINER_CLASS(SpoilerCol);

}  // namespace gh