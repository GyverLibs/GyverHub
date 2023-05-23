#pragma once
#include "config.h"
#include "index.h"
#include "macro.h"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_WS
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
    void beginHTTP() {
        server.on("/hub_discover_all", [this]() {
            server.sendHeader(F("Access-Control-Allow-Private-Network"), F("true"));
            server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
            server.sendHeader(F("Access-Control-Allow-Methods"), F("*"));
            server.send(200, F("text/plain"), F("OK"));
        });

#ifndef GH_NO_PORTAL
        server.on("/", [this]() {
            server.sendHeader(F("Content-Encoding"), F("gzip"));
            server.send_P(200, "text/html", (PGM_P)hub_index_gz, (size_t)hub_index_gz_len);
        });
        server.on("/favicon.svg", [this]() {
            server.send(200);
        });

        server.serveStatic("/", GH_FS, "/hub/", GH_CACHE_PRD);
        /*server.on("/favicon.svg", [this]() {
            File f = GH_FS.open("/hub/favicon.svg.gz", "r");
            if (f) server.streamFile(f, "image/svg+xml");
        });
        server.on("/test.html", [this]() {
            File f = GH_FS.open("/hub/test.html.gz", "r");
            if (f) server.streamFile(f, "text/html");
        });*/
        server.on("/script.js", [this]() {
            File f = GH_FS.open("/hub/script.js.gz", "r");
            if (f) server.streamFile(f, "text/javascript");
        });
        server.on("/style.css", [this]() {
            File f = GH_FS.open("/hub/style.css.gz", "r");
            if (f) server.streamFile(f, "text/css");
        });
#endif

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