#pragma once
#include "config.h"
#include "macro.h"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_LOCAL
class HubHTTP {
   public:
};
#else

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#else
#include <WebServer.h>
#include <WiFi.h>
#endif

class HubHTTP {
   public:
    HubHTTP() : server(GH_HTTP_PORT) {}

   protected:
    virtual String answerDiscover() = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void setStatus(GHstate_t state, GHconn_t conn) = 0;

    void beginHTTP() {
        server.onNotFound([this]() {
            uint8_t ok = 0;
            String str('/');
            str += getPrefix();
            if (server.uri() == str) ok = 1;
            str += '/';
            str += getID();
            if (server.uri() == str) ok = 2;

            if (ok) {
                server.sendHeader(F("Access-Control-Allow-Private-Network"), F("true"));
                server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
                server.send(200, "text/plain", answerDiscover());
                setStatus(ok == 1 ? GH_DISCOVER_ALL : GH_DISCOVER, GH_HTTP);
            } else {
                server.send(404);
                setStatus(GH_UNKNOWN, GH_HTTP);
            }
        });

        server.begin(GH_HTTP_PORT);
    }
    void endHTTP() {
        server.stop();
    }
    void tickHTTP() {
        server.handleClient();
    }

   private:
#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif
};
#endif
#endif