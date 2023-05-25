#pragma once
#include "config.h"
#include "macro.h"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_WS
class HubHTTP {
   public:
};
#else

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

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
            resp->addHeader(F("Access-Control-Allow-Private-Network"), F("true"));
            resp->addHeader(F("Access-Control-Allow-Origin"), F("*"));
            resp->addHeader(F("Access-Control-Allow-Methods"), F("*"));
            req->send(resp);
        });

#ifndef GH_NO_PORTAL
        /*server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", hub_index_gz, hub_index_gz_len);
            gzipHeader(response);
            request->send(response);
        });*/
        server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/index.html.gz", "text/html");
            gzipHeader(response);
            request->send(response);
        });
        server.on("/favicon.svg", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(200);
            request->send(response);
        });

        server.serveStatic("/", GH_FS, "/hub/").setCacheControl(GH_CACHE_PRD);
        /*server.on("/favicon.svg", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/favicon.svg.gz", "image/svg+xml");
            gzipHeader(response);
            request->send(response);
        });
        server.on("/test.html", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/test.html.gz", "text/html");
            gzipHeader(response);
            request->send(response);
        });*/
        server.on("/script.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/script.js.gz", "text/javascript");
            gzipHeader(response);
            request->send(response);
        });
        server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, "/hub/style.css.gz", "text/css");
            gzipHeader(response);
            request->send(response);
        });
#endif

        server.begin();
    }

    void endHTTP() {
        server.end();
    }

    void tickHTTP() {
    }

    // ============ PRIVATE =============
   private:
    void gzipHeader(AsyncWebServerResponse* response) {
        response->addHeader("Content-Encoding", "gzip");
    }
};
#endif
#endif