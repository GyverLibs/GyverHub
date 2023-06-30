#pragma once
#include "../config.hpp"
#include "../macro.hpp"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_WS
class HubWS {
   public:
};
#else

#include <Arduino.h>
#include <WebSocketsServer.h>

#include "../utils/stats.h"

class HubWS {
    // ============ PROTECTED =============
   protected:
    HubWS() : ws(GH_WS_PORT, "", "hub") {}

    virtual void parse(char* url, GHconn_t from) = 0;
    virtual void sendEvent(GHevent_t state, GHconn_t from) = 0;

    void beginWS() {
        ws.onEvent([this](uint8_t num, WStype_t type, uint8_t* data, GH_UNUSED size_t len) {
            switch (type) {
                case WStype_CONNECTED:
                    sendEvent(GH_CONNECTED, GH_WS);
                    break;

                case WStype_DISCONNECTED:
                    sendEvent(GH_DISCONNECTED, GH_WS);
                    break;

                case WStype_ERROR:
                    sendEvent(GH_ERROR, GH_WS);
                    break;

                case WStype_TEXT: {
                    clientID = num;
                    /*char buf[len + 1];
                    memcpy(buf, data, len);
                    buf[len] = 0;*/
                    parse((char*)data, GH_WS);
                } break;

                case WStype_BIN:
                case WStype_FRAGMENT_TEXT_START:
                case WStype_FRAGMENT_BIN_START:
                case WStype_FRAGMENT:
                case WStype_FRAGMENT_FIN:
                case WStype_PING:
                case WStype_PONG:
                    break;
            }
        });

        ws.begin();
    }

    void endWS() {
        ws.close();
    }

    void tickWS() {
        ws.loop();
    }

    void sendWS(const String& answ) {
        ws.broadcastTXT(answ.c_str(), answ.length());
    }

    void answerWS(const String& answ) {
        ws.sendTXT(clientID, answ.c_str(), answ.length());
    }

    // ============ PRIVATE =============
   private:
    WebSocketsServer ws;
    uint8_t clientID = 0;
};
#endif
#endif