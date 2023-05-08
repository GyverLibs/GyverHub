#pragma once
#include "config.h"
#include "macro.h"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_LOCAL
class HubHTTP {
   public:
};
#else

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class HubHTTP {
    // ============ PUBLIC =============
   public:
    HubHTTP() : server(GH_HTTP_PORT) {}

    // ============ PROTECTED =============
   protected:
    virtual String answerDiscover() = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void setStatus(GHstate_t state, GHconn_t conn) = 0;

    void beginHTTP() {
        server.on("/*", HTTP_GET, [this](AsyncWebServerRequest* req) {
            uint8_t ok = 0;
            String str('/');
            str += getPrefix();
            if (req->url() == str) ok = 1;
            str += '/';
            str += getID();
            if (req->url() == str) ok = 2;

            if (ok) {
                String answ = answerDiscover();
                AsyncWebServerResponse* resp = req->beginResponse(200, "text/plain", answ);
                resp->addHeader(F("Access-Control-Allow-Private-Network"), F("true"));
                resp->addHeader(F("Access-Control-Allow-Origin"), F("*"));
                resp->addHeader(F("Access-Control-Allow-Methods"), F("*"));
                req->send(resp);
                setStatus(ok == 1 ? GH_DISCOVER_ALL : GH_DISCOVER, GH_HTTP);
            } else {
                req->send(404);
                setStatus(GH_UNKNOWN, GH_HTTP);
            }
        });

        server.begin();
    }

    void endHTTP() {
        server.end();
    }

    void tickHTTP() {
    }

    // ============ PRIVATE =============
   private:
    AsyncWebServer server;
};
#endif
#endif