#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "bridge.h"
#include "client_class.h"
#include "hooks.h"
#include "hub_macro.hpp"
#include "types.h"

namespace gh {

class Client {
   public:
    Client(void* hub = nullptr,
           ghc::SendHook hook = nullptr,
           Bridge* bridge = nullptr,
           uint32_t id = 0) : bridge(bridge),
                              connection(bridge ? bridge->connection : Connection::System),
                              id(id),
                              _hub(hub),
                              _send(hook) {
    }

    // мост клиента
    Bridge* bridge;

    // тип подключения
    const Connection connection;

    // id клиента
    const uint32_t id = 0;

    // отправить текст клиенту
    void send(GHTREF text) {
        if (_hub && _send) _send(_hub, text, this);
    }

    bool operator==(Client& client) {
        return (client.bridge == bridge && client.id == id);
    }

    void* _hub;
    bool _sent = false;

   private:
    ghc::SendHook _send;
};

}  // namespace gh