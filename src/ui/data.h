#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/client.h"
#include "hub_macro.hpp"

namespace gh {

struct Data {
    GHTREF name;
    GHTREF value;
    Client& client;
};

}  // namespace gh