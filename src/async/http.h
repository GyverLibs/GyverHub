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
    virtual String answerDiscover(bool broadcast) = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void setStatus(GHstate_t state, GHconn_t conn) = 0;

    void beginHTTP() {
        String str('/');
        str += getPrefix();
        server.on(str.c_str(), HTTP_GET, [this](AsyncWebServerRequest* req) {
            String answ = answerDiscover(true);
            AsyncWebServerResponse* resp = req->beginResponse(200, "text/plain", answ);
            setHeaders(resp);
            req->send(resp);
            setStatus(GH_DISCOVER_ALL, GH_HTTP);
        });

        str += '/';
        str += getID();
        server.on(str.c_str(), HTTP_GET, [this](AsyncWebServerRequest* req) {
            String answ = answerDiscover(false);
            AsyncWebServerResponse* resp = req->beginResponse(200, "text/plain", answ);
            setHeaders(resp);
            req->send(resp);
            setStatus(GH_DISCOVER, GH_HTTP);
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
    void setHeaders(AsyncWebServerResponse* resp) {
        resp->addHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        resp->addHeader(F("Access-Control-Allow-Origin"), F("*"));
        resp->addHeader(F("Access-Control-Allow-Methods"), F("*"));
    }
    AsyncWebServer server;
};
#endif
#endif