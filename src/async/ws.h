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
#include <ESPAsyncWebServer.h>

#include "../utils/stats.h"

class HubWS {
    // ============ PROTECTED =============
   protected:
    HubWS() : server(GH_WS_PORT), ws("/") {
        server.addHandler(&ws);
    }

    virtual void parse(char* url, GHconn_t from) = 0;
    virtual void sendEvent(GHevent_t state, GHconn_t from) = 0;

    void beginWS() {
        ws.onEvent([this](GH_UNUSED AsyncWebSocket* server, GH_UNUSED AsyncWebSocketClient* client, AwsEventType etype, void* arg, uint8_t* data, size_t len) {
            switch (etype) {
                case WS_EVT_CONNECT:
                    sendEvent(GH_CONNECTED, GH_WS);
                    break;

                case WS_EVT_DISCONNECT:
                    sendEvent(GH_DISCONNECTED, GH_WS);
                    break;

                case WS_EVT_ERROR:
                    sendEvent(GH_ERROR, GH_WS);
                    break;

                case WS_EVT_DATA: {
                    AwsFrameInfo* ws_info = (AwsFrameInfo*)arg;
                    if (ws_info->final && ws_info->index == 0 && ws_info->len == len && ws_info->opcode == WS_TEXT) {
                        clientID = client->id();
                        parse((char*)data, GH_WS);
                    }
                } break;

                case WS_EVT_PONG:
                    break;
            }
        });

        server.begin();
    }

    void endWS() {
        server.end();
    }

    void tickWS() {
        ws.cleanupClients();
    }

    void sendWS(const String& answ) {
        ws.textAll(answ.c_str());
    }

    void answerWS(String& answ) {
        ws.text(clientID, answ.c_str());
    }

    // ============ PRIVATE =============
   private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    uint32_t clientID = 0;
};
#endif
#endif