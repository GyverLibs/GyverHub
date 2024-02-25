#pragma once
#include <Arduino.h>
#include <Stream.h>
#include <StringUtils.h>

#include "bridge.h"

namespace gh {

class HubStream : public Bridge {
   public:
    HubStream() {}
    HubStream(void* hub, ghc::ParseHook parse) : Bridge(hub, Connection::System, parse) {}

    void setup(void* hub, ghc::ParseHook hook) {
        _hub = hub;
        _parse = hook;
        Bridge::disable();
    }

    // настроить Stream
    void config(Stream* stream, Connection conn) {
        _stream = stream;
        _conn = conn;
        Bridge::enable();
    }

    // отключить
    void end() {
        _stream = nullptr;
        Bridge::disable();
    }

    void tick() {
        if (_stream && _stream->available()) {
            String str = _stream->readStringUntil('\0');
            parse(str);
        }
    }

    void send(BridgeData& data) {
        Serial.print(data.text);
    }

   private:
    Stream* _stream = nullptr;
};

}  // namespace gh