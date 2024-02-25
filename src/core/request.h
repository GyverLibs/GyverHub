#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "client.h"
#include "request_class.h"
#include "types.h"

namespace ghc {
typedef bool (*RequestCallback)(gh::Request& request);
}

namespace gh {

class Request {
   public:
    Request(Client& client,
            CMD cmd,
            GHTREF name = GHTXT(),
            GHTREF value = GHTXT()) : client(client),
                                      cmd(cmd),
                                      name(name),
                                      value(value) {}

    // клиент
    Client& client;

    // событие
    const CMD cmd;

    // имя
    GHTREF name;

    // значение
    GHTREF value;
};

}  // namespace gh