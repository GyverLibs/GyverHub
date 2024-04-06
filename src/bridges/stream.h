#pragma once
#include <Arduino.h>
#include <GyverHub.h>
#include <Stream.h>
#include <StringUtils.h>

#include "core/bridge.h"

namespace gh {

class BridgeStream : public Bridge {
   public:
    BridgeStream(void* hub, Stream* stream, Connection conn) : Bridge(hub, conn, GyverHub::parseHook), _stream(stream) {}

    // настроить Stream
    void config(Stream* stream, Connection conn) {
        _stream = stream;
        connection = conn;
    }

    // отключить
    void end() {
        _stream = nullptr;
    }

    void tick() {
        if (_stream && canParse() && _stream->available()) {
            parse(_stream->readStringUntil('\0'));
        }
    }

    void send(BridgeData& data) {
        if (_stream) _stream->print(data.text);
    }

   private:
    Stream* _stream = nullptr;
};

}  // namespace gh