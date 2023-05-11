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
    virtual String answerDiscover(bool broadcast) = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void setStatus(GHstate_t state, GHconn_t conn) = 0;

    void beginHTTP() {
        String str('/');
        str += getPrefix();
        server.on(str.c_str(), [this]() {
            setHeaders();
            server.send(200, "text/plain", answerDiscover(true));
            setStatus(GH_DISCOVER_ALL, GH_HTTP);
        });

        str += '/';
        str += getID();
        server.on(str.c_str(), [this]() {
            setHeaders();
            server.send(200, "text/plain", answerDiscover(false));
            setStatus(GH_DISCOVER, GH_HTTP);
        });

        server.onNotFound([this]() {
            server.send(404);
            setStatus(GH_UNKNOWN, GH_HTTP);
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
    void setHeaders() {
        server.sendHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
        server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
    }
#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif
};
#endif
#endif