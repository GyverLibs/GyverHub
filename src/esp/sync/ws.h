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
    HubWS() : ws(GH_WS_PORT, "", "hub") {}

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
                    clientID = num;
                    // parse(GHTXT((char*)data, len));

                    if (url_buf) delete url_buf;
                    url_buf = strdup((char*)data);
                    url_len = len;
                    url_id = num;
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
            clientID = url_id;
            parse(GHTXT(url_buf, url_len));
            delete url_buf;
            url_buf = nullptr;
        }
    }

    void send(gh::BridgeData& data) {
        if (data.text.pgm()) {
            char buf[data.text.length()];
            data.text.toStr(buf);
            if (data.broadcast) {
                ws.broadcastTXT((uint8_t*)buf, data.text.length());
            } else {
                ws.sendTXT(clientID, (uint8_t*)buf, data.text.length());
            }
        } else {
            if (data.broadcast) {
                ws.broadcastTXT(data.text.str(), data.text.length());
            } else {
                ws.sendTXT(clientID, data.text.str(), data.text.length());
            }
        }
        yield();
    }

    // ======================= PRIVATE =======================
    WebSocketsServer ws;
    uint8_t clientID = 0;

    char* url_buf = nullptr;
    size_t url_len;
    uint8_t url_id;
};

}  // namespace ghc
#endif