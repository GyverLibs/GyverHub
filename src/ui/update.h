#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/client.h"
#include "core/hub.h"
#include "core/packet.h"
#include "widget.h"

namespace gh {

class Update {
   public:
    Update(GyverHub* hub, Client* client = nullptr) : widget(p), _hub(hub), _client(client) {
        widget._allowed = _hub->canSend();
        if (widget._allowed) _init();
    }

    // начать обновление по имени виджета (или список) + имя функции
    ghc::Widget& update(GHTREF name) {
        if (widget._allowed) {
            if (_first) _first = false;
            else p.endObj();
            p.beginObj(name);
        }
        return widget;
    }

    // отправить пакет
    void send() {
        if (widget._allowed) {
            if (!_first) p.endObj();
            p.endObj();
            p.addID(_hub->id);
            p.addClient(_client);
            p.addString(ghc::Tag::type, ghc::Tag::update);
            p.endPacket();
            _hub->_send(p, _client);
            _init();
        }
    }

    // текущий виджет для установки значений
    ghc::Widget widget;

   private:
    GyverHub* _hub;
    Client* _client;
    ghc::Packet p;
    bool _first = true;

    void _init() {
        _first = true;
        p.beginPacket();
        p.beginObj(ghc::Tag::updates);
    }
};

}  // namespace gh