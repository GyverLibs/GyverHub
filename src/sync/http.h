#pragma once
#include "../config.hpp"
#include "../macro.hpp"

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

#ifdef GH_INCLUDE_PORTAL
#include "../esp_inc/index.h"
#include "../esp_inc/script.h"
#include "../esp_inc/style.h"
#endif

class HubHTTP {
   public:
#ifdef ESP8266
    ESP8266WebServer server;
#else
    WebServer server;
#endif

    HubHTTP() : server(GH_HTTP_PORT) {}

   protected:
    void beginHTTP() {
        server.on("/hub_discover_all", [this]() {
            server.send(200, F("text/plain"), F("OK"));
        });

#ifndef GH_NO_PORTAL
        server.on("/favicon.svg", [this]() {
            server.send(200);
        });

#ifdef GH_INCLUDE_PORTAL
        server.on("/", [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/html", (PGM_P)hub_index_h, (size_t)hub_index_h_len);
        });
        server.on("/script.js", [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/javascript", (PGM_P)hub_script_h, (size_t)hub_script_h_len);
        });
        server.on("/style.css", [this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/css", (PGM_P)hub_style_h, (size_t)hub_style_h_len);
        });
#else
#ifndef GH_NO_FS
        server.on("/", [this]() {
            File f = GH_FS.open("/hub/index.html.gz", "r");
            if (f) server.streamFile(f, "text/html");
        });
        server.on("/script.js", [this]() {
            cache_h();
            File f = GH_FS.open("/hub/script.js.gz", "r");
            if (f) server.streamFile(f, "text/javascript");
        });
        server.on("/style.css", [this]() {
            cache_h();
            File f = GH_FS.open("/hub/style.css.gz", "r");
            if (f) server.streamFile(f, "text/css");
        });
#endif
#endif
#endif
        server.begin(GH_HTTP_PORT);
        server.enableCORS(true);
    }
    void endHTTP() {
        server.stop();
    }
    void tickHTTP() {
        server.handleClient();
    }

   private:
    void gzip_h() {
        server.sendHeader(F("Content-Encoding"), F("gzip"));
    }
    void cache_h() {
        server.sendHeader(F("Cache-Control"), GH_CACHE_PRD);
    }
};
#endif
#endif