#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "canvas.h"
#include "core/client.h"
#include "core/hub.h"
#include "core/packet.h"

namespace gh {

class CanvasUpdate : public Canvas {
   public:
    CanvasUpdate(GHTREF name, GyverHub* hub, Client* client = nullptr) : _hub(hub), _client(client) {
        if (!_hub->canSend()) return;
        Canvas::setBuffer(&p);
        p.beginPacket();
        p.s += F("\"updates\":{");
        p.addKey(name);
        p.s += F("{\"data\":[");
    }

    // отправить
    void send() {
        if (!_hub->canSend()) return;
        p.endArr();
        p.endObj();
        p.endObj();
        p.addID(_hub->id);
        p.addClient(_client);
        p.addString(ghc::Tag::type, ghc::Tag::update);
        p.endPacket();
        _hub->_send(p, _client);
    }

   private:
    ghc::Packet p;
    GyverHub* _hub;
    Client* _client = nullptr;
};

}  // namespace gh