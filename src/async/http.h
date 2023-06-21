#pragma once
#include "../config.hpp"
#include "../macro.hpp"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_WS
class HubHTTP {
   public:
};
#else

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#ifndef GH_NO_FS
#if (GH_FS == LittleFS)
#include <LittleFS.h>
#elif (GH_FS == SPIFFS)
#include <SPIFFS.h>
#endif
#endif

#ifdef GH_INCLUDE_PORTAL
#include "../esp_inc/index.h"
#include "../esp_inc/script.h"
#include "../esp_inc/style.h"
#endif

class HubHTTP {
    // ============ PUBLIC =============
   public:
    AsyncWebServer server;

    HubHTTP() : server(GH_HTTP_PORT) {}

    // ============ PROTECTED =============
   protected:
    void beginHTTP() {
        server.on("/hub_discover_all", HTTP_GET, [this](AsyncWebServerRequest* req) {
            AsyncWebServerResponse* resp = req->beginResponse(200, F("text/plain"), F("OK"));
            req->send(resp);
        });

#ifndef GH_NO_PORTAL
        server.on("/favicon.svg", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(200);
            request->send(response);
        });
#ifdef GH_INCLUDE_PORTAL
        server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", hub_index_h, hub_index_h_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
        server.on("/script.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse_P(200, "text/javascript", hub_script_h, hub_script_h_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
        server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse_P(200, "text/css", hub_style_h, hub_style_h_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
#else
#ifndef GH_NO_FS
        server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/index.html.gz", "text/html");
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
        server.on("/script.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/script.js.gz", "text/javascript");
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
        server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/style.css.gz", "text/css");
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
#endif
#endif
#endif
        CORS();
        server.begin();
    }

    void endHTTP() {
        server.end();
    }

    void tickHTTP() {
    }

    // ============ PRIVATE =============
   private:
    void CORS() {
        DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), F("*"));
        DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Private-Network"), F("true"));
        DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Methods"), F("*"));
    }
    void gzip_h(AsyncWebServerResponse* response) {
        response->addHeader("Content-Encoding", "gzip");
    }
    void cache_h(AsyncWebServerResponse* response) {
        response->addHeader(F("Cache-Control"), GH_CACHE_PRD);
    }
};
#endif
#endif