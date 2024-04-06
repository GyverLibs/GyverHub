#pragma once
#include <Arduino.h>

#include "core/client.h"
#include "core/packet.h"
#include "core/types.h"

namespace ghc {

class TransferBase {
   public:
    TransferBase(gh::Client& client, uint32_t id, Tag errtype) : client(client), id(id), errtype(errtype) {}

    gh::Client client;
    uint32_t id;
    Tag errtype;
    gh::Error error = gh::Error::None;

    void setError(gh::Error err) {
        error = err;
        sendError(client, id, err, errtype);
    }

    bool hasError() {
        return error != gh::Error::None;
    }

    bool canSend() {
        return client.bridge && id;
    }

    static void sendError(gh::Client& client, uint32_t id, gh::Error err, Tag errtype) {
        if (client.bridge && id) {
            Packet p(50, &client);
            p.beginPacket(id);
            p.addString(Tag::type, errtype);
            p.addInt(Tag::code, (uint8_t)err);
            p.endPacket();
            p.send();
        }
    }

    void answerCMD(Tag cmd) {
        if (client.bridge && id) {
            Packet p(50, &client);
            p.beginPacket(id);
            p.addString(Tag::type, cmd);
            p.endPacket();
            p.send();
        }
    }
};

}  // namespace ghc