#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"

#ifdef GH_ESP_BUILD

#include "core/core_class.h"
#include "core/fs.h"
#include "core/hooks.h"
#include "core/hub_class.h"
#include "core/modules.h"
#include "core/request.h"
#include "core/types.h"
#include "utils/crc32.h"
#include "utils/mime.h"

#ifndef GH_NO_HTTP_TRANSFER
#ifndef GH_NO_HTTP_OTA
#include "esp/transfer/updater.h"
#endif

#ifndef GH_NO_FS

#ifndef GH_NO_HTTP_FETCH
#include "esp/transfer/fetcher.h"
#endif

#ifndef GH_NO_HTTP_UPLOAD
#include "esp/transfer/uploader.h"
#endif

#endif  // GH_NO_FS
#endif  // GH_NO_HTTP_TRANSFER

#ifdef ESP8266
#include <ESP8266WebServer.h>
#define GH_SERVER_T ESP8266WebServer
#else
#include <WebServer.h>
#define GH_SERVER_T WebServer
#endif

#ifndef GH_NO_HTTP_DNS
#include <DNSServer.h>
#endif

#ifdef GH_INCLUDE_PORTAL
#include "esp/esp_h/index.h"
#include "esp/esp_h/script.h"
#include "esp/esp_h/style.h"
#endif

namespace ghc {

class HubHTTP : public gh::Bridge {
    friend class HubCore;
    friend class ::GyverHub;

   public:
    GH_SERVER_T server;

    // ======================= PRIVATE =======================
   private:
    HubHTTP() : server(GH_HTTP_PORT) {}

    void setup(void* hub, ParseHook prh, FetchHook fh, RequestHook rh, UploadHook uh, bool* safe_upl, void* mod, gh::Reboot* reason) {
        gh::Bridge::config(hub, gh::Connection::HTTP, prh);
        this->fetch_h = fh;
        this->request_h = rh;
        this->upload_h = uh;
        this->safe_upl = safe_upl;
        this->modules = mod;
        this->reason = reason;
    }

    void send(gh::BridgeData& data) {
        if (!handled) {
            handled = 1;
            server.setContentLength(CONTENT_LENGTH_UNKNOWN);
            server.send(200, "text/plain");
        }
        if (data.text.pgm()) server.sendContent_P(data.text.str(), data.text.length());
        else server.sendContent(data.text.str(), data.text.length());
    }

    void begin() {
        server.onNotFound([this]() {
            String url = server.uri().substring(5);  // 5 == /hub/
            handled = 0;
            parse(url);
            if (handled) {
                server.sendContent("");
                server.client().stop();
                return;
            }
// fetch file from GH_PUBLIC_PATH
#if !defined(GH_NO_HTTP_PUBLIC) && !defined(GH_NO_HTTP_FETCH) && !defined(GH_NO_HTTP_TRANSFER) && !defined(GH_NO_FS)
            if (server.uri().indexOf('.') > 0) {
                String path(GH_PUBLIC_PATH);
                path += server.uri();
                _handleFetch(path);
                return;
            }
#endif

// captive portal
#ifndef GH_NO_HTTP_DNS
#if defined(GH_INCLUDE_PORTAL)
            gzip_h();
            cache_h();
            server.send_P(200, "text/html", (PGM_P)hub_index_h, (size_t)hub_index_h_len);
#elif defined(GH_FILE_PORTAL)
            File f = gh::FS.openRead(F("/hub/index.html.gz"));
            if (f) server.streamFile(f, "text/html");
            else _upload_portal();
#endif
#endif  // GH_NO_HTTP_DNS
        });

// fetch /hub/fetch?path=...&client_id=...
#if !defined(GH_NO_HTTP_FETCH) && !defined(GH_NO_HTTP_TRANSFER) && !defined(GH_NO_FS)
        server.on("/hub/fetch", HTTP_GET, [this]() {
            String path = server.arg(F("path"));
            _handleFetch(path);
        });
#endif

// upload /hub/upload_portal
#if !defined(GH_NO_HTTP_UPLOAD_PORTAL) && !defined(GH_NO_FS)
        server.on("/hub/upload_portal", HTTP_GET, [this]() {
            _upload_portal();
        });

        server.on(
            "/hub/upload_portal", HTTP_POST, [this]() { _upload_portal(); }, [this]() {
                HTTPUpload& upload = server.upload();

                switch (upload.status) {
                    case UPLOAD_FILE_START: {
                        String path(F("/hub/"));
                        gh::FS.mkdir(path.c_str());
                        path += upload.filename;
                        file_upl = gh::FS.openWrite(path.c_str());
                        if (!file_upl) server.send(400);
                    } break;

                    case UPLOAD_FILE_WRITE:
                        if (file_upl) {
                            size_t bytesWritten = file_upl.write(upload.buf, upload.currentSize);
                            if (bytesWritten != upload.currentSize) server.send(400);
                        }
                        break;

                    case UPLOAD_FILE_END:
                    case UPLOAD_FILE_ABORTED:
                        if (file_upl) file_upl.close();
                        break;
                } });
#endif

// upload /hub/upload?path=...&crc32=...&client_id=...&size=...
#if !defined(GH_NO_HTTP_UPLOAD) && !defined(GH_NO_HTTP_TRANSFER) && !defined(GH_NO_FS)
        server.on(
            "/hub/upload", HTTP_POST, [this]() {
                    if (_upl_p) {
                        if (_upl_p->hasError()) _err(_upl_p->error);
                        else _ok();
GHDELPTR(_upl_p);
                    } else _err(gh::Error::End); }, [this]() {
#ifndef GH_NO_MODULES
                if (!((Modules*)modules)->read(gh::Module::ModUpload)) return _err(gh::Error::Disabled);
#endif
                HTTPUpload& upload = server.upload();
                String client_id = server.arg(F("client_id"));
                gh::Client client(_hub, nullptr, this, client_id);

                switch (upload.status) {
                    case UPLOAD_FILE_START:
                        if (!_upl_p) {
                            String path = server.arg(F("path"));
                            String crc = server.arg(F("crc32"));
                            String size = server.arg(F("size"));
#ifndef GH_NO_REQUEST
                            gh::Request req(client, gh::CMD::Upload, path);
                            if (!request_h(_hub, &req)) return _err(gh::Error::Forbidden);
#endif
                            _upl_p = new Uploader(client, nullptr, *safe_upl);
                            if (_upl_p) {
                                _upl_p->setCRC(crc);
                                if (!_upl_p->begin(path, size)) {
                                    _err(_upl_p->error);
                                    GHDELPTR(_upl_p);
                                }
                            } else _err(gh::Error::Memory);
                        } else _err(gh::Error::Busy);
                        break;

                    case UPLOAD_FILE_WRITE:
                    case UPLOAD_FILE_END:
                        if (_upl_p) {
                            if (upload.status == UPLOAD_FILE_WRITE) {
                                if (!_upl_p->write(upload.buf, upload.currentSize)) _err(_upl_p->error);
                            } else {    // UPLOAD_FILE_END
                                if (_upl_p->end()) upload_h(_hub, _upl_p->path);
                                else _err(_upl_p->error);
                            }
                        } else _err(gh::Error::WrongClient);
                        break;

                    case UPLOAD_FILE_ABORTED:
                        if (_upl_p) _upl_p->abort();
                        break;
                } });
#endif

// ota /hub/ota?type=...&client_id=...
#if !defined(GH_NO_HTTP_OTA) && !defined(GH_NO_HTTP_TRANSFER)
        server.on(
            "/hub/ota", HTTP_POST, [this]() {
                    server.sendHeader(F("Connection"), F("close"));
                    if (_ota_p) {
                        if (_ota_p->hasError()) _err(_ota_p->error);
                        else _ok();
                        GHDELPTR(_ota_p);
                    } else _err(gh::Error::End); },
            [this]() {
#ifndef GH_NO_MODULES
                if (!((Modules*)modules)->read(gh::Module::ModOta)) return _err(gh::Error::Disabled);
#endif
                HTTPUpload& upload = server.upload();
                String client_id = server.arg(F("client_id"));
                gh::Client client(_hub, nullptr, this, client_id);

                switch (upload.status) {
                    case UPLOAD_FILE_START:
                        if (!_ota_p) {
                            String type = server.arg(F("type"));
#ifndef GH_NO_REQUEST
                            gh::Request req(client, gh::CMD::Upload, type);
                            if (!request_h(_hub, &req)) _err(gh::Error::Forbidden);
#endif
                            _ota_p = new Updater(client, reason, nullptr);
                            if (_ota_p) {
                                if (!_ota_p->begin(type)) {
                                    _err(_ota_p->error);
                                    GHDELPTR(_ota_p);
                                }
                            } else _err(gh::Error::Memory);
                        } else _err(gh::Error::Busy);
                        break;

                    case UPLOAD_FILE_WRITE:
                    case UPLOAD_FILE_END:
                        if (_ota_p) {
                            if (upload.status == UPLOAD_FILE_WRITE) {
                                if (!_ota_p->write(upload.buf, upload.currentSize)) _err(_ota_p->error);
                            } else {
                                if (!_ota_p->end()) _err(_ota_p->error);
                            }
                        } else _err(gh::Error::WrongClient);
                        break;

                    case UPLOAD_FILE_ABORTED:
                        if (_ota_p) _ota_p->abort();
                        break;
                }
                yield();
            });
#endif

// PORTAL_H
#if defined(GH_INCLUDE_PORTAL)
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

// PORTAL FILE
#elif defined(GH_FILE_PORTAL) && !defined(GH_NO_FS)
        server.on("/", [this]() {
            File f = gh::FS.openRead(F("/hub/index.html.gz"));
            if (f) server.streamFile(f, "text/html");
            else _upload_portal();
        });
        server.on("/script.js", [this]() {
            cache_h();
            File f = gh::FS.openRead(F("/hub/script.js.gz"));
            if (f) server.streamFile(f, F("text/javascript"));
            else server.send(404);
        });
        server.on("/style.css", [this]() {
            cache_h();
            File f = gh::FS.openRead(F("/hub/style.css.gz"));
            if (f) server.streamFile(f, F("text/css"));
            else server.send(404);
        });
#endif

// DNS for captive portal
#if (defined(GH_INCLUDE_PORTAL) || defined(GH_FILE_PORTAL)) && !defined(GH_NO_HTTP_DNS)
        if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
            dns_f = 1;
            dns.start(53, "*", WiFi.softAPIP());
        }
#endif

        server.begin(GH_HTTP_PORT);
        server.enableCORS(true);
    }

    void end() {
        server.stop();
#ifndef GH_NO_HTTP_DNS
        if (dns_f) dns.stop();
#endif
    }

    void tick() {
        server.handleClient();
#ifndef GH_NO_HTTP_DNS
        if (dns_f) dns.processNextRequest();
#endif

#if !defined(GH_NO_HTTP_UPLOAD) && !defined(GH_NO_HTTP_TRANSFER) && !defined(GH_NO_FS)
        if (_upl_p && _upl_p->timeout()) GHDELPTR(_upl_p);
#endif
#if !defined(GH_NO_HTTP_OTA) && !defined(GH_NO_HTTP_TRANSFER) && !defined(GH_NO_FS)
        if (_ota_p && _ota_p->timeout()) GHDELPTR(_upl_p);
#endif
    }

    // ======================= PRIVATE =======================
    FetchHook fetch_h;
    RequestHook request_h;
    UploadHook upload_h;
    void* modules;
    bool handled = 0;
    gh::Reboot* reason;
    bool* safe_upl;

    void gzip_h() {
        server.sendHeader(F("Content-Encoding"), F("gzip"));
    }
    void cache_h() {
        server.sendHeader(F("Cache-Control"), F(GH_CACHE_PRD));
    }

#if !defined(GH_NO_HTTP_FETCH) && !defined(GH_NO_HTTP_TRANSFER) && !defined(GH_NO_FS)
    void _handleFetch(String& path) {
#ifndef GH_NO_MODULES
        if (!((Modules*)modules)->read(gh::Module::ModFetch)) return _err(gh::Error::Disabled);
#endif
        String client_id = server.arg(F("client_id"));
        gh::Client client(_hub, nullptr, this, client_id);
#ifndef GH_NO_REQUEST
        gh::Request req(client, gh::CMD::Fetch, path);
        if (!request_h(_hub, &req)) return _err(gh::Error::Forbidden);
#endif
        gh::Fetcher fetch(client, fetch_h, path, nullptr);

        if (fetch.begin()) {
            String mime = getMime(path);
            if (fetch.bytes) {
                server.setContentLength(fetch.len);
                server.send(200, mime, "");
                if (fetch.pgm) server.sendContent_P((PGM_P)fetch.bytes, fetch.len);
                else server.sendContent((PGM_P)fetch.bytes, fetch.len);
            } else if (fetch.file) {
                server.streamFile(fetch.file, mime);
            }
            fetch.end();
        } else {
            _err(fetch.error);
        }
    }
#endif

    void _err(gh::Error error) {
        char b[3];
        itoa((uint8_t)error, b, DEC);
        server.send(400, F("text/plain"), b);
    }
    void _ok() {
        server.send(200, F("text/plain"), "0");
    }
    void _upload_portal() {
        server.send(200, F("text/html"), F(R"raw(<form method='POST' action='/hub/upload_portal' enctype='multipart/form-data'><input type='file' name='update' multiple><input type='submit' value='Upload'></form><br><a href='/'>Home</a>)raw"));
    }

#if !defined(GH_NO_HTTP_OTA) && !defined(GH_NO_HTTP_TRANSFER)
    Updater* _ota_p = nullptr;
#endif

#ifndef GH_NO_FS

#if !defined(GH_NO_HTTP_UPLOAD) && !defined(GH_NO_HTTP_TRANSFER)
    Uploader* _upl_p = nullptr;
#endif

#ifndef GH_NO_HTTP_UPLOAD_PORTAL
    File file_upl;
#endif

#endif  // GH_NO_FS

#ifndef GH_NO_HTTP_DNS
    bool dns_f = false;
    DNSServer dns;
#endif
};
}  // namespace ghc
#endif