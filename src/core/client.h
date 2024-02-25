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
           GHTREF id = GHTXT()) : bridge(bridge),
                                  _hub(hub),
                                  _send(hook) {
        if (id.valid() && id.length() <= 8) {
            id.toStr(_id);
            this->id = GHTXT(_id, id.length());
        }
    }

    // тип подключения
    Connection connection() {
        return bridge ? bridge->connection() : Connection::System;
    }

    // id клиента
    GHTXT id;

    bool operator==(Client& client) {
        return (client.bridge == bridge && client.id == id);
    }

    Bridge* bridge;

    // отправить текст клиенту
    void send(GHTREF text) {
        if (_hub && _send) _send(_hub, text, this);
    }

    void* _hub;

   private:
    char _id[9] = {0};
    ghc::SendHook _send;
};

}  // namespace gh