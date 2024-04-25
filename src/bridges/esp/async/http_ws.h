#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <GyverHub.h>
#include <StringUtils.h>

#include "core/bridge.h"

namespace gh {

class HubHttpWs : public gh::Bridge {
   public:
    HubHttpWs(GyverHub* hub, uint16_t port = 80) : Bridge(hub, Connection::HTTP_WS, GyverHub::parseHook), server(port), ws("/") {
        setPort(port);
    }

    void begin() {
        // ws
        ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
            switch (type) {
                case WS_EVT_CONNECT:
                    setFocus();
                    break;
                case WS_EVT_DISCONNECT:
                    clearFocus();
                    break;
                case WS_EVT_DATA:
                    clientID = client->id();
                    parse(sutil::AnyText(data, len));
                    break;
                default:
                    break;
            }
        });
        server.addHandler(&ws);

        // http
        server.begin();
        server.on("/hub", HTTP_GET, [this](AsyncWebServerRequest* request) {
            String req = request->url().substring(5);
            AsyncResponseStream* response = request->beginResponseStream(F("text/plain"));
            response->addHeader(F("Access-Control-Allow-Origin"), "*");
            response->addHeader(F("Access-Control-Allow-Private-Network"), "*");
            response->addHeader(F("Access-Control-Allow-Methods"), "*");
            resp_p = response;
            parse(req);
            resp_p = nullptr;
            request->send(response);
        });
    }

    void end() {
        ws.closeAll();
        server.end();
    }

    void tick() {
        ws.cleanupClients();
    }

    void send(BridgeData& data) {
        if (resp_p) {
            resp_p->write(data.text.str(), data.text.length());
        } else {
            if (data.broadcast) {
                ws.textAll(data.text.str(), data.text.length());
            } else {
                ws.text(clientID, data.text.str(), data.text.length());
            }
        }
    }

    AsyncWebServer server;
    AsyncWebSocket ws;

   private:
    uint32_t clientID = 0;
    AsyncResponseStream* resp_p = nullptr;
};

}  // namespace gh