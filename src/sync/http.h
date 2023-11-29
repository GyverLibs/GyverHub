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

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <uri/UriBraces.h>
#else
#include <WebServer.h>
#include <WiFi.h>
#include <uri/UriBraces.h>
#ifndef GH_NO_HTTP_OTA
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

#ifndef GH_NO_FS
        server.on("/hub_http_cfg", [this]() {
            server.send(200, F("text/plain"), F("{\"upload\":" GH_HTTP_UPLOAD ",\"download\":" GH_HTTP_DOWNLOAD ",\"ota\":" GH_HTTP_OTA ",\"path\":\"" GH_HTTP_PATH "\"}"));
        });

#ifndef GH_NO_HTTP_DOWNLOAD
        server.on(UriBraces(GH_HTTP_PATH "{}"), [this]() {
            String path(F("/fs/"));
            path += server.pathArg(0);
            File f = GH_FS.open(path, "r");
            if (f) server.streamFile(f, getMime(path));
        });
#endif

#ifndef GH_NO_HTTP_UPLOAD
        server.on(
            "/upload", HTTP_POST, [this]() { server.send(200, F("text/plain"), F("OK")); }, [this]() {
                HTTPUpload& upload = server.upload();
                if (upload.status == UPLOAD_FILE_START) {
                    _fsmakedir(upload.filename.c_str());
                    file = GH_FS.open(upload.filename, "w");
                    if (!file) server.send(500, F("text/plain"), F("FAIL"));
                } else if (upload.status == UPLOAD_FILE_WRITE) {
                    if (file) {
                        size_t bytesWritten = file.write(upload.buf, upload.currentSize);
                        if (bytesWritten != upload.currentSize) server.send(500, F("text/plain"), F("FAIL"));
                    }
                } else if (upload.status == UPLOAD_FILE_END) {
                    if (file) file.close();
                } });
#endif

#ifndef GH_NO_HTTP_OTA
        server.on(
            "/ota", HTTP_POST, [this]() {
        server.sendHeader(F("Connection"), F("close"));
        server.send(200, F("text/plain"), Update.hasError() ? F("FAIL") : F("OK"));
        _rebootOTA(); },
            [this]() {
                HTTPUpload& upload = server.upload();
                if (upload.status == UPLOAD_FILE_START) {
                    int ota_type = 0;
                    if (server.args()) {
                        if (!strcmp_P(server.arg(0).c_str(), PSTR("flash"))) ota_type = 1;
                        else if (!strcmp_P(server.arg(0).c_str(), PSTR("fs"))) ota_type = 2;
                    } else {
                        if (!strcmp_P(upload.name.c_str(), PSTR("flash"))) ota_type = 1;
                        else if (!strcmp_P(upload.name.c_str(), PSTR("fs"))) ota_type = 2;
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
                        Update.begin(ota_size, ota_type);
                    }
                } else if (upload.status == UPLOAD_FILE_WRITE) {
                    Update.write(upload.buf, upload.currentSize);
                } else if (upload.status == UPLOAD_FILE_END) {
                    Update.end(true);
                }
                yield();
            });
#endif
#endif

#if defined(GH_INCLUDE_PORTAL) && !defined(GH_NO_DNS)
        if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
            dns_f = 1;
            dns.start(53, "*", WiFi.softAPIP());
        }
        server.onNotFound([this]() {
            gzip_h();
            cache_h();
            server.send_P(200, "text/html", (PGM_P)hub_index_h, (size_t)hub_index_h_len);
        });
#endif

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
#ifndef GH_NO_DNS
        if (dns_f) dns.stop();
#endif
    }
    void tickHTTP() {
        server.handleClient();
        if (dns_f) dns.processNextRequest();
    }

   protected:
    virtual void _rebootOTA() = 0;
    virtual void _fsmakedir(const char* path) = 0;

   private:
    void gzip_h() {
        server.sendHeader(F("Content-Encoding"), F("gzip"));
    }
    void cache_h() {
        server.sendHeader(F("Cache-Control"), GH_CACHE_PRD);
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