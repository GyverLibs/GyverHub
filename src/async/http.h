#pragma once
#include "../config.hpp"
#include "../macro.hpp"
#include "../utils/mime.h"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_WS
class HubHTTP {
   public:
};
#else

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#ifndef GH_NO_HTTP_OTA
#ifdef ESP32
#include <Update.h>
#endif
#endif

#ifndef GH_NO_FS
#if (GH_FS == LittleFS)
#include <LittleFS.h>
#elif (GH_FS == SPIFFS)
#include <SPIFFS.h>
#endif
#endif

#ifndef GH_NO_DNS
#include <DNSServer.h>
#endif

#ifdef GH_INCLUDE_PORTAL
#include "../esp_inc/index.h"
#include "../esp_inc/script.h"
#include "../esp_inc/style.h"
#endif

#define GH_HTTP_UPLOAD "1"
#define GH_HTTP_DOWNLOAD "1"
#define GH_HTTP_OTA "1"

#ifdef GH_NO_HTTP_UPLOAD
#define GH_HTTP_UPLOAD "0"
#endif
#ifdef GH_NO_HTTP_DOWNLOAD
#define GH_HTTP_DOWNLOAD "0"
#endif
#ifdef GH_NO_HTTP_OTA
#define GH_HTTP_OTA "0"
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

#ifndef GH_NO_FS
        server.on("/hub_http_cfg", HTTP_GET, [this](AsyncWebServerRequest* req) {
            AsyncWebServerResponse* resp = req->beginResponse(200, F("text/plain"), F("{\"upload\":" GH_HTTP_UPLOAD ",\"download\":" GH_HTTP_DOWNLOAD ",\"ota\":" GH_HTTP_OTA ",\"path\":\"" GH_HTTP_PATH "\"}"));
            req->send(resp);
        });

#ifndef GH_NO_HTTP_DOWNLOAD
        server.on(GH_HTTP_PATH "*", HTTP_GET, [this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse(GH_FS, request->url(), getMime(request->url()));
            request->send(response);
        });
#endif

#ifndef GH_NO_HTTP_UPLOAD
        server.on(
            "/upload", HTTP_POST, [this](AsyncWebServerRequest* request) { request->send(200, F("text/plain"), F("OK")); },
            [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
                if (!index) {
                    _fsmakedir(filename.c_str());
                    file = GH_FS.open(filename, "w");
                    if (!file) {
                        AsyncWebServerResponse* resp = request->beginResponse(500, F("text/plain"), F("FAIL"));
                        request->send(resp);
                    }
                }
                if (len) {
                    if (file) {
                        size_t bytesWritten = file.write(data, len);
                        if (bytesWritten != len) {
                            AsyncWebServerResponse* resp = request->beginResponse(500, F("text/plain"), F("FAIL"));
                            request->send(resp);
                        }
                    }
                }
                if (final) {
                    if (file) file.close();
                }
            });
#endif

#ifndef GH_NO_HTTP_OTA
        server.on(
            "/ota", HTTP_POST, [this](AsyncWebServerRequest* request) { 
                AsyncWebServerResponse* resp = request->beginResponse(200, F("text/plain"), Update.hasError() ? F("FAIL") : F("OK"));
                request->send(resp);
                _rebootOTA();
                },
            [this](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
                if (!index) {
                    int ota_type = 0;
                    if (request->params()) {
                        AsyncWebParameter* p = request->getParam(0);
                        if (!strcmp_P(p->value().c_str(), PSTR("flash"))) ota_type = 1;
                        else if (!strcmp_P(p->value().c_str(), PSTR("fs"))) ota_type = 2;
                    }

                    if (ota_type) {
                        size_t ota_size;
                        if (ota_type == 1) {
                            ota_type = U_FLASH;
#ifdef ESP8266
                            ota_size = (size_t)((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
#else
                            ota_size = UPDATE_SIZE_UNKNOWN;
#endif
                        } else {
#ifdef ESP8266
                            ota_type = U_FS;
                            close_all_fs();
                            ota_size = (size_t)&_FS_end - (size_t)&_FS_start;
#else
                            ota_type = U_SPIFFS;
                            ota_size = UPDATE_SIZE_UNKNOWN;
#endif
                        }
#ifdef ESP8266
                        Update.runAsync(true);
#endif
                        Update.begin(ota_size, ota_type);
                    }
                }
                if (len) {
                    Update.write(data, len);
                }
                if (final) {
                    Update.end(true);
                }
            });
#endif
#endif

#if defined(GH_INCLUDE_PORTAL) && !defined(GH_NO_DNS)
        if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
            dns_f = 1;
            dns.start(53, "*", WiFi.softAPIP());
        }
        server.onNotFound([this](AsyncWebServerRequest* request) {
            AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", hub_index_h, hub_index_h_len);
            gzip_h(response);
            cache_h(response);
            request->send(response);
        });
#endif

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
#ifndef GH_NO_DNS
        if (dns_f) dns.stop();
#endif
    }

    void tickHTTP() {
        if (dns_f) dns.processNextRequest();
    }

   protected:
    virtual void _rebootOTA() = 0;
    virtual void _fsmakedir(const char* path) = 0;

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
    String getMime(const String& path) {
        for (uint16_t i = 0; i < GH_MIME_AMOUNT; i++) {
            if (path.endsWith(FPSTR(_GH_mimie_ex_list[i]))) return FPSTR(_GH_mimie_list[i]);
        }
        return F("text/plain");
    }
    File file;

#ifndef GH_NO_DNS
    bool dns_f = false;
    DNSServer dns;
#endif
};
#endif
#endif