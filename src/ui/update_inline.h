#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/client.h"
#include "core/core.h"
#include "core/hooks.h"
#include "core/packet.h"
#include "widget.h"

namespace gh {

class UpdateInline : public ghc::Widget {
   public:
    UpdateInline(bool allowed,
                 const char* id,
                 GHTREF name,
                 GHTREF func,
                 Client client) : ghc::Widget(p),
                                  p(50, &_client),
                                  _client(client) {
        _allowed = allowed;
        if (!_allowed) return;
        p.beginPacket(id, &_client);
        p.addString(ghc::Tag::type, ghc::Tag::update);
        p.beginObj(ghc::Tag::updates);
        p.beginObj(name);
        if (func.valid()) p.addString(ghc::Tag::func, func);
    }

    ~UpdateInline() {
        if (!_allowed) return;
        p.endObj();
        p.endObj();
        p.endPacket();
        p.send();
    }

   private:
    ghc::Packet p;
    Client _client;
};

}  // namespace gh