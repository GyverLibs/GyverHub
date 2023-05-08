#pragma once

/*
#include "config.h"
#include "macro.h"

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
*/
/*
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
    virtual void setState(GHstate_t stat) = 0;
    virtual void setFrom(GHconn_t conn) = 0;

    void beginHTTP() {
        server.onNotFound([this]() {
            setFrom(GH_HTTP);
            setState(GH_UNKNOWN);

            bool ok = false;
            String str('/');
            str += getPrefix();
            if (server.uri() == str) setState(GH_DISCOVER_ALL), ok = 1;
            str += '/';
            str += getID();
            if (server.uri() == str) setState(GH_DISCOVER), ok = 1;

            if (ok) {
                server.sendHeader(F("Access-Control-Allow-Private-Network"), F("true"));
                server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
                server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
                server.send(200, "text/plain", answerDiscover());
            } else {
                server.send(404);
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
    ESP8266WebServer server;
};
*/
/*
#ifdef GH_ESP_BUILD
#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#else
#include <WiFi.h>
#endif
#include "utils/stats.h"

class HubHTTP {
   public:
   HubHTTP() : server(GH_HTTP_PORT) {}

   protected:
    virtual String answerDiscover() = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void setState(GHstate_t stat) = 0;
    virtual void setFrom(GHconn_t conn) = 0;

    void beginHTTP() {
        server.begin();
    }
    void endHTTP() {
        server.stop();
    }
    void tickHTTP() {
        WiFiClient client = server.available();  // accept() in 3+
        if (client) {
            setFrom(GH_HTTP);
            setState(GH_UNKNOWN);
            client.readStringUntil('/');  // "GET /"
            String req = client.readStringUntil('\r');
            while (client.available()) client.read();
            int end = req.indexOf(F(" HTTP/"));
            if (end < 0) return;
            req = req.substring(0, end);

            bool ok = false;
            String str(getPrefix());
            if (req == str) setState(GH_DISCOVER_ALL), ok = 1;
            str += '/';
            str += getID();
            if (req == str) setState(GH_DISCOVER), ok = 1;

            if (ok) {
                client.print(F(
                    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
                    "Access-Control-Allow-Origin:*\r\n"
                    "Access-Control-Allow-Private-Network: true\r\n"
                    "Access-Control-Allow-Methods:*\r\n\r\n"));
                client.print(answerDiscover());
            } else {
                client.print(F("HTTP/1.1 404 Not Found\r\n"));
            }
        }
    }

   private:
    WiFiServer server;
};
#endif
*/