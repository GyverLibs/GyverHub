#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"

#ifdef GH_ESP_BUILD
#include <WebSocketsServer.h>

#include "core/bridge.h"
#include "core/core_class.h"
#include "core/hooks.h"
#include "core/hub_class.h"
#include "core/types.h"

namespace ghc {

class HubWS : public gh::Bridge {
    friend class HubCore;
    friend class ::GyverHub;

   private:
    HubWS(uint16_t port = GH_WS_PORT) : ws(port, "", "hub") {
        setPort(port);
    }

    using Bridge::setPort;

    void setup(void* hub, ParseHook hook) {
        gh::Bridge::config(hub, gh::Connection::WS, hook);
    }

    void begin() {
        ws.onEvent([this](uint8_t num, WStype_t type, uint8_t* data, size_t len) {
            switch (type) {
                case WStype_CONNECTED:
                    setFocus();
                    break;

                case WStype_DISCONNECTED:
                    clearFocus();
                    break;

                case WStype_TEXT:
                    // parse(GHTXT((char*)data, len));

                    if (url_buf) delete[] url_buf;
                    url_buf = new char[len];
                    if (!url_buf) return;
                    memcpy(url_buf, data, len);
                    url_len = len;

                    clientID = num;
                    break;

                default:
                    break;
            }
        });

        ws.begin();
    }

    void end() {
        ws.close();
    }

    void tick() {
        ws.loop();

        if (url_buf) {
            parse(GHTXT(url_buf, url_len));
            delete[] url_buf;
            url_buf = nullptr;
        }
    }

    void send(gh::BridgeData& data) {
        if (data.broadcast) {
            ws.broadcastTXT(data.text.str(), data.text.length());
        } else {
            ws.sendTXT(clientID, data.text.str(), data.text.length());
        }
        yield();
    }

    // ======================= PRIVATE =======================
    WebSocketsServer ws;
    uint8_t clientID = 0;

    char* url_buf = nullptr;
    size_t url_len;
};

}  // namespace ghc
#endif