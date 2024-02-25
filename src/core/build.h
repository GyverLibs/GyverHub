#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "client.h"
#include "hub_macro.hpp"
#include "types.h"
#include "ui/color.h"
#include "ui/flags.h"
#include "ui/pos.h"

namespace gh {

struct Build {
    Build(Action action,
          Client& client,
          GHTREF name = GHTXT(),
          GHTREF value = GHTXT()) : action(action),
                                    client(client),
                                    name(name),
                                    value(value) {}

    // тип билда
    const Action action;

    // клиент
    Client& client;

    // имя компонента
    const GHTXT name;

    // значение компонента
    const GHTXT value;

    // билд-действие
    bool isSet() {
        return action == Action::Set;
    }

    // билд-запрос виджетов
    bool isUI() {
        return action == Action::UI;
    }

    // получить значение как Color
    Color valueColor() {
        return Color(value.toInt32(), HEX);
    }

    // получить значение как Flags
    Flags valueFlags() {
        return Flags(value.toInt16());
    }

    // получить значение как Pos
    Pos valuePos() {
        uint32_t xy = value.toInt32();
        return Pos(xy >> 16, xy & 0xffff, true);
    }
};

}  // namespace gh