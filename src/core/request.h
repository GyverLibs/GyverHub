#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "client.h"
#include "request_class.h"
#include "types.h"

namespace gh {

class Request {
   public:
    Request(Client& client,
            bool broadcast,
            CMD cmd,
            GHTREF name = GHTXT(),
            GHTREF value = GHTXT()) : client(client),
                                      broadcast(broadcast),
                                      cmd(cmd),
                                      name(name),
                                      value(value) {}

    // клиент
    Client& client;

    // широковещательный запрос
    bool broadcast = false;

    // событие
    const CMD cmd;

    // имя
    const GHTXT name;

    // значение
    const GHTXT value;
};

}  // namespace gh