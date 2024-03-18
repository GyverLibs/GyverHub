#pragma once

#include <Arduino.h>
#include <StringUtils.h>

#include "bridge.h"
#include "build.h"
#include "client.h"
#include "cmd.h"
#include "core_class.h"
#include "datatypes.h"
#include "fs.h"
#include "hooks.h"
#include "hub_macro.hpp"
#include "modules.h"
#include "packet.h"
#include "request.h"
#include "types.h"
#include "types_read.h"
#include "ui/builder.h"
#include "ui/info.h"

#ifdef GH_ESP_BUILD

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif  // ESP8266

#ifdef ESP32
#include <WiFi.h>
#endif  // ESP32

#include "esp/transfer/fetcher.h"

#ifndef GH_NO_OTA
#include "esp/transfer/updater.h"
#endif

#ifndef GH_NO_OTA_URL
#include "esp/transfer/ota_url.h"
#endif

#ifndef GH_NO_FS
#ifndef GH_NO_UPLOAD
#include "esp/transfer/uploader.h"
#endif
#endif  // GH_NO_FS

#ifndef GH_NO_HTTP
#include "esp/sync/http.h"
#endif
#ifndef GH_NO_MQTT
#include "esp/sync/mqtt.h"
#endif
#ifndef GH_NO_WS
#include "esp/sync/ws.h"
#endif

#endif  // GH_ESP_BUILD

#ifndef GH_NO_STREAM
#include "stream.h"
#endif

namespace ghc {
#ifdef GH_ESP_BUILD
typedef std::function<void(gh::Data data)> DataCallback;
typedef std::function<void(String str)> CliCallback;
typedef std::function<void(uint32_t stamp)> UnixCallback;
typedef std::function<void(gh::Builder& builder)> BuildCallback;
typedef std::function<bool(gh::Request& request)> RequestCallback;

typedef std::function<void(gh::Reboot res)> RebootCallback;
typedef std::function<void(gh::Fetcher& fetcher)> FetchCallback;
typedef std::function<void(String& path)> UploadCallback;
#else
typedef void (*DataCallback)(gh::Data data);
typedef void (*CliCallback)(String str);
typedef void (*UnixCallback)(uint32_t stamp);
typedef void (*BuildCallback)(gh::Builder& builder);
typedef bool (*RequestCallback)(gh::Request& request);
#endif

// ========================== CLASS ==========================
class HubCore {
   public:
    // добавить мост подключения
    bool addBridge(gh::Bridge* bridge) {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (!bridges[i]) {
                bridges[i] = bridge;
                return 1;
            }
        }
        return 0;
    }

    // хук парсинга
    static void parseHook(void* hub, gh::Bridge& bridge, GHTREF url, GHTREF data) {
        ((HubCore*)hub)->_parse(bridge, url, data);
    }

    // отправить текст клиенту
    void _send(GHTREF text, gh::Client* client = nullptr) {
        if (!text.length()) return;
        gh::BridgeData data(text);
        if (client && client->bridge) {
            data.broadcast = false;
            if (client->connection() == gh::Connection::MQTT) {
                data.topic = _topicSend(client->id);
            }
            client->bridge->send(data);
        } else {
            data.broadcast = true;
            data.topic = _topicSend();
            for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
                if (bridges[i] && bridges[i]->getFocus()) bridges[i]->send(data);
            }
        }
    }

    // VARS
    uint8_t menu = 0;  // выбранный пункт меню
    String prefix = "";
    String name = "";
    String icon = "";
    String version = "";
    const char* id = _id;
    gh::Bridge* bridges[GH_BRIDGE_AMOUNT] = {nullptr};

// модули
#ifndef GH_NO_MODULES
    Modules modules;
#endif

#ifndef GH_NO_STREAM
    gh::HubStream stream;
#endif

#if defined(GH_ESP_BUILD) && !defined(GH_NO_MQTT)
    HubMQTT mqtt;
#endif

    // ==========================================================================
    // ================================= PRIVATE ================================
    // ==========================================================================
   protected:
    char _id[9] = {'\0'};
    uint32_t _pin = 0;

    uint16_t _bufsize = 1000;
    bool _running_f = false;
    bool _build_busy = false;  // (билдер запущен) запрещает вызывать функции, которые вызывают билдер
    bool _allow_send = true;   // разрешает отправку sendXxx и класс Update только вне билдера + в Set билдере
    bool _autoGet_f = true;

    BuildCallback _build_cb = nullptr;
    CliCallback _cli_cb = nullptr;
    InfoCallback _info_cb = nullptr;
    UnixCallback _unix_cb = nullptr;
#ifndef GH_NO_REQUEST
    RequestCallback _req_cb = nullptr;
#endif

#ifdef GH_ESP_BUILD
    bool _safe_upl = true;
    UploadCallback _upload_cb = nullptr;
    FetchCallback _fetch_cb = nullptr;
    RebootCallback _reboot_cb = nullptr;
    gh::Reboot _reason = gh::Reboot::None;

#ifndef GH_NO_FS
#ifndef GH_NO_FETCH
    gh::Fetcher* _fet_p = nullptr;
#endif
#ifndef GH_NO_UPLOAD
    Uploader* _upl_p = nullptr;
#endif
#endif  // GH_NO_FS

#ifndef GH_NO_OTA
    Updater* _ota_p = nullptr;
#endif
#ifndef GH_NO_OTA_URL
    OtaUrl* _otaurl_p = nullptr;
#endif

#ifndef GH_NO_HTTP
    HubHTTP http;
#endif
#ifndef GH_NO_WS
    HubWS ws;
#endif
#endif

    // ==========================================================================
    // ================================== FUNC ==================================
    // ==========================================================================

    void _init() {
#ifndef GH_NO_STREAM
        stream.setup(this, parseHook);
        addBridge(&stream);
#endif

#ifdef GH_ESP_BUILD
#ifndef GH_NO_HTTP
#ifndef GH_NO_MODULES
        http.setup(this, parseHook, _fetchHook, _requestHook, _uploadHook, &_safe_upl, &modules, &_reason);
#else
        http.setup(this, parseHook, _fetchHook, _requestHook, _uploadHook, &_safe_upl, nullptr, &_reason);
#endif
        addBridge(&http);
#endif  // GH_NO_HTTP

#ifndef GH_NO_WS
        ws.setup(this, parseHook);
        addBridge(&ws);
#endif
#ifndef GH_NO_MQTT
        mqtt.setup(this, parseHook, _id, &prefix);
        addBridge(&mqtt);
#endif
#endif
    }

    // ==========================================================================
    // ================================== HOOKS =================================
    // ==========================================================================
    static void sendHook(void* hub, GHTREF text, gh::Client* client) {
        ((HubCore*)hub)->_send(text, client);
    }

#ifdef GH_ESP_BUILD
    static void _fetchHook(void* hubptr, gh::Fetcher* fetcher) {
        if (((HubCore*)hubptr)->_fetch_cb) ((HubCore*)hubptr)->_fetch_cb(*fetcher);
    }
    static void _uploadHook(void* hubptr, String& path) {
        if (((HubCore*)hubptr)->_upload_cb) ((HubCore*)hubptr)->_upload_cb(path);
    }
    static bool _requestHook(void* hubptr, gh::Request* request) {
#ifndef GH_NO_REQUEST
        if (((HubCore*)hubptr)->_req_cb) return ((HubCore*)hubptr)->_req_cb(*request);
#endif
        return 1;
    }
#endif  // GH_ESP_BUILD

    // ==========================================================================
    // ================================= ANSWER =================================
    // ==========================================================================

    void _answerDiscover(gh::Client& client) {
#ifndef GH_NO_MODULES
#ifdef GH_ESP_BUILD
#ifdef GH_NO_FS
        modules.clear(gh::Module::ModFiles | gh::Module::ModFormat | gh::Module::ModFetch | gh::Module::ModUpload | gh::Module::ModDelete | gh::Module::ModRename | gh::Module::ModCreate);
#endif
#ifdef GH_NO_OTA
        modules.clear(gh::Module::ModOta);
#endif
#ifdef GH_NO_OTA_URL
        modules.clear(gh::Module::ModOtaUrl);
#endif
#else
        modules.clear(gh::Module::ModReboot | gh::Module::ModFiles | gh::Module::ModFormat | gh::Module::ModFetch | gh::Module::ModUpload | gh::Module::ModOta | gh::Module::ModOtaUrl | gh::Module::ModDelete | gh::Module::ModRename | gh::Module::ModCreate);
#endif
#endif
        uint32_t hash = 0;
        if (_pin > 999) {
            char pin_s[11];
            ultoa(_pin, pin_s, 10);
            uint16_t len = strlen(pin_s);
            for (uint16_t i = 0; i < len; i++) {
                hash = (((uint32_t)hash << 5) - hash) + pin_s[i];
            }
        }

        Packet p(200, &client, _bufsize);
        p.beginPacket(id);
        p.addString(Tag::type, Tag::discover);
        p.addString(Tag::name, name);
        p.addString(Tag::prefix, prefix);
        p.addString(Tag::icon, icon);
        p.addInt(Tag::PIN, hash);
        p.addString(Tag::version, version);
        p.addString(Tag::platform, F(GH_PLATFORM));
        p.addInt(Tag::max_upl, GH_UPL_CHUNK_SIZE);
        p.addInt(Tag::api_v, GH_API_VERSION);

#if defined(GH_NO_HTTP_TRANSFER)
        p.addInt(Tag::http_t, 0);
#else
        p.addInt(Tag::http_t, 1);
#endif
#ifdef ATOMIC_FS_UPDATE
        p.addString(Tag::ota_t, F("gz"));
#else
        p.addString(Tag::ota_t, F("bin"));
#endif
#ifndef GH_NO_MODULES
        p.addInt(Tag::ws_port, GH_WS_PORT);  // TODO WS
        p.addInt(Tag::modules, modules.mods);
#else
        p.addInt(Tag::ws_port, GH_WS_PORT);
        p.addInt(Tag::modules, 0);
#endif
        p.endPacket();
        p.send();
    }
    void _answerAck(GHTREF name, gh::Client& client) {
        Packet p(50, &client);
        p.beginPacket(id);
        p.addString(Tag::type, Tag::ack);
        p.addString(Tag::name, name);
        p.endPacket();
        p.send();
    }
    void _answerError(Tag cmd, gh::Error err, gh::Client& client) {
        Packet p(50, &client);
        p.beginPacket(id);
        p.addString(Tag::type, cmd);
        p.addInt(Tag::code, (uint8_t)err);
        p.endPacket();
        p.send();
    }
    void _answerCmd(Tag type, gh::Client& client) {
        Packet p(50, &client);
        p.beginPacket(id);
        p.addString(Tag::type, type);
        p.endPacket();
        p.send();
    }
    void _answerInfo(gh::Client& client) {
        Packet p(300, &client, _bufsize);
        p.beginPacket(id);
        p.addString(Tag::type, Tag::info);
        gh::Info::_build(_info_cb, p, version, client);
        p.endPacket();
        p.send();
    }
    void _answerUI(gh::Client& client) {
        Packet p(50, &client, _bufsize);
        p.beginPacket(id);
        p.addString(Tag::type, Tag::ui);
        p.beginArr(Tag::controls);
#ifndef GH_NO_MODULES
        if (modules.read(gh::Module::ModUI)) _uiBuild(p, client);
#else
        _uiBuild(p, client);
#endif
        p.endArr();
        p.endPacket();
        p.send();
    }
    void _answerFiles(gh::Client& client) {
#if !defined(GH_NO_FS)
        if (!gh::FS.mounted()) return _answerCmd(Tag::fs_err, client);
        uint32_t count = 0;
        {
            String str;
            str.reserve(100);
            gh::FS.showFiles(str, "/", GH_FS_DEPTH, &count);
        }
        Packet p(count + 50, &client, _bufsize);
        p.s += F("#{\"fs\":{\"/\":0,");
        gh::FS.showFiles(p, "/", GH_FS_DEPTH);
        p.endObj();

        p.addID(id);
        p.addString(Tag::type, Tag::files);
        p.addInt(Tag::used, gh::FS.usedSpace());
        p.addInt(Tag::total, gh::FS.totalSpace());
        p.endPacket();
        p.send();
#endif
    }

    // ==========================================================================
    // ================================== SEND ==================================
    // ==========================================================================

    void _sendUpdate(GHTREF name, GHTREF value, gh::Client* client = nullptr) {
        Packet p(50);
        p.beginPacket(id, client);
        p.addString(Tag::type, Tag::update);
        p.beginObj(Tag::updates);
        p.beginObj(name);
        p.addStringEsc(Tag::value, value);
        p.endObj();
        p.endObj();
        p.endPacket();
        _send(p, client);
    }
    void _sendGet(GHTREF name, GHTREF text) {
        if (!_running_f || !_allow_send) return;
        String topic = _topicGet(name);
        _sendMQTT(topic, text);
    }
    void _sendMQTT(String& topic, GHTREF text) {
        gh::BridgeData data(text);
        data.topic = topic;
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i] && bridges[i]->connection() == gh::Connection::MQTT) {
                bridges[i]->send(data);
            }
        }
    }
    // prefix/hub
    String _topicSend() {
        String topic(prefix);
        topic += F("/hub");
        return topic;
    }
    // prefix/hub/client_id/id
    String _topicSend(GHTREF clientID) {
        String topic = _topicSend();
        topic += '/';
        clientID.addString(topic);
        topic += '/';
        topic += id;
        return topic;
    }
    // prefix/hub/id/get/name
    String _topicGet(GHTREF name) {
        String topic(prefix);
        topic += F("/hub/");
        topic += id;
        topic += F("/get/");
        name.addString(topic);
        return topic;
    }
    // prefix/hub/id/status
    String _topicStatus() {
        String topic(prefix);
        topic += F("/hub/");
        topic += id;
        topic += F("/status");
        return topic;
    }

    // ==========================================================================
    // ================================= BUILD ==================================
    // ==========================================================================

    bool _readBuild(Packet& p, gh::Client& client, GHTREF name) {
        if (_build_cb) {
            gh::Build build(gh::Action::Read, client, name);
            gh::Builder b(build, &p);
            _build_busy = true;
            _allow_send = false;
            _build_cb(b);
            _allow_send = true;
            _build_busy = false;
            return b._stop;
        }
        return 0;
    }
    void _uiBuild(Packet& p, gh::Client& client) {
        if (!_build_cb) return;
        _build_busy = true;
        _allow_send = false;
        p.reserve(_bufsize);
        gh::Build build(gh::Action::UI, client);
        gh::Builder b(build, &p);
        b._menu = &menu;
        _build_cb(b);
        if (!b._first) p.endObj();
        _allow_send = true;
        _build_busy = false;
    }

#ifndef GH_NO_REQUEST
    bool _request(gh::Client& client, gh::CMD cmd, GHTREF name = GHTXT(), GHTREF value = GHTXT()) {
        if (!_req_cb) return 1;
        gh::Request req(client, cmd, name, value);
        return _req_cb(req);
    }
#endif

    void _discover(gh::Bridge& bridge, GHTREF id, gh::CMD cmd) {
        gh::Client client(this, sendHook, &bridge, id);
#ifndef GH_NO_REQUEST
        if (_request(client, cmd)) {
            _answerDiscover(client);
        } else {
            _answerError(Tag::error, gh::Error::Forbidden, client);
        }
#else
        _answerDiscover(client);
#endif
    }

    // ==========================================================================
    // ================================== PARSE =================================
    // ==========================================================================

    // PREFIX/ID/CLIENT_ID/CMD/NAME + VALUE
    void _parse(gh::Bridge& bridge, GHTREF url, GHTREF value) {
        if (url == prefix) return _discover(bridge, value, gh::CMD::Search);

        sutil::SplitterT<5> sp((char*)url.str(), '/');
        if (prefix != sp.str(0)) return;    // wrong prefix
        if (strcmp(sp.str(1), id)) return;  // wrong id
        if (sp.length() == 2) return _discover(bridge, value, gh::CMD::Discover);
        if (sp.length() == 3) return;

        gh::CMD cmd = getCMD(sp.str(3));
        if (cmd == gh::CMD::Unknown) return;

        GHTXT name = sp.get(4);

#ifdef GH_ESP_BUILD
        size_t nameHash = sutil::hash(name.str());
#endif
        gh::Client client(this, sendHook, &bridge, sp.str(2));

#ifndef GH_NO_REQUEST
        if (!_request(client, cmd, name, value)) return _answerError(Tag::error, gh::Error::Forbidden, client);
#endif
#ifndef GH_NO_MODULES
        if (!modules.checkCMD(cmd)) return _answerError(Tag::error, gh::Error::Disabled, client);
#endif

        bridge.setFocus();

        switch (cmd) {
            case gh::CMD::UI:
                _answerUI(client);
                break;

            case gh::CMD::Ping:
                _answerCmd(Tag::OK, client);
                break;

            case gh::CMD::Unfocus:
                bridge.clearFocus();
                break;

            case gh::CMD::Info:
#ifdef GH_NO_INFO
                _answerCmd(Tag::OK, client);
#else
                _answerInfo(client);
#endif
                break;

            case gh::CMD::Data:
                // TODO data
                break;

            case gh::CMD::Unix:  // name = stamp
                if (_unix_cb) _unix_cb(name.toInt32());
                _answerCmd(Tag::OK, client);
                break;

            case gh::CMD::Set: {
                if (!_build_cb) return _answerCmd(Tag::OK, client);
                gh::Build build(gh::Action::Set, client, name, value);
                gh::Builder b(build);
                b._menu = &menu;
                _build_busy = true;
                _build_cb(b);
                _build_busy = false;

                if (b.isRefresh()) _answerUI(client);
                else _answerAck(name, client);

                if (b._needs_update) {
#ifndef GH_NO_GET
                    if (_autoGet_f) _sendGet(name, value);
#endif
                    _sendUpdate(name, value);
                }
            } break;

#ifndef GH_NO_GET
            case gh::CMD::Get: {
                Packet p(50, &client);
                p.beginPacket(id);
                p.addString(Tag::type, Tag::get);
                p.addKey(Tag::value);
                p.quotes();
                _readBuild(p, client, name);
                p.quotes();
                p.endPacket();
                p.send();
            } break;
#endif

            case gh::CMD::Read: {
                Packet p(0, &client);
                _readBuild(p, client, name);
#ifndef GH_NO_GET
                if (_autoGet_f) _sendGet(name, p);
#endif
                p.send();
            } break;

            case gh::CMD::CLI:
                _answerCmd(Tag::OK, client);
                if (_cli_cb) _cli_cb(value.toString());
                break;

#ifdef GH_ESP_BUILD
            case gh::CMD::Reboot:
                _reason = gh::Reboot::Button;
                _answerCmd(Tag::OK, client);
                break;

#ifndef GH_NO_FS
            case gh::CMD::Files:
                _answerFiles(client);
                break;

            case gh::CMD::Format:
                gh::FS.format();
                gh::FS.end();
                gh::FS.begin();
                _answerFiles(client);
                break;

            case gh::CMD::Delete:
                gh::FS.remove(name.str());
                _answerFiles(client);
                break;

            case gh::CMD::Rename:
                if (gh::FS.rename(name.str(), value.str())) {
                    _answerFiles(client);
                }
                break;

            case gh::CMD::Create: {  // name == path
                gh::FS.mkdir(value.str());
                File f = gh::FS.openWrite(name.str());
                f.close();
                _answerFiles(client);
            } break;

            case gh::CMD::FsAbort:
                switch (nameHash) {  // name == type
#ifndef GH_NO_UPLOAD
                    case sutil::SH("upload"):
                        if (_upl_p && _upl_p->client == client) {
                            _upl_p->abort();
                            GHDELPTR(_upl_p);
                        }
                        break;
#endif  // GH_NO_UPLOAD
#ifndef GH_NO_FETCH
                    case sutil::SH("fetch"):
                    case sutil::SH("fetch_file"):
                        if (_fet_p && _fet_p->client == client) {
                            _fet_p->abort();
                            GHDELPTR(_fet_p);
                        }
                        break;
#endif  // GH_NO_FETCH
#ifndef GH_NO_OTA
                    case sutil::SH("ota"):
                        if (_ota_p && _ota_p->client == client) {
                            _ota_p->abort();
                            GHDELPTR(_ota_p);
                        }
                        break;
#endif  // GH_NO_OTA
                    case sutil::SH("all"):
#ifndef GH_NO_UPLOAD
                        if (_upl_p && _upl_p->client == client) {
                            _upl_p->abort();
                            GHDELPTR(_upl_p);
                        }
#endif  // GH_NO_UPLOAD
#ifndef GH_NO_FETCH
                        if (_fet_p && _fet_p->client == client) {
                            _fet_p->abort();
                            GHDELPTR(_fet_p);
                        }
#endif  // GH_NO_FETCH
#ifndef GH_NO_OTA
                        if (_ota_p && _ota_p->client == client) {
                            _ota_p->abort();
                            GHDELPTR(_ota_p);
                        }
#endif  // GH_NO_OTA
                        break;
                    default:
                        break;
                }
                break;

#ifndef GH_NO_FETCH
            case gh::CMD::Fetch:  // name == path
                if (!_fet_p) {
                    _fet_p = new gh::Fetcher(client, _fetchHook, name, _id);
                    if (_fet_p) {
                        if (!_fet_p->begin()) GHDELPTR(_fet_p);
                    } else {
                        gh::Fetcher::sendError(client, _id, gh::Error::Memory);
                    }
                } else {
                    gh::Fetcher::sendError(client, _id, gh::Error::Busy);
                }
                break;

            case gh::CMD::FetchNext:
                if (_fet_p && _fet_p->client == client) {
                    if (_fet_p->next()) GHDELPTR(_fet_p);  // last
                } else {
                    gh::Fetcher::sendError(client, _id, gh::Error::WrongClient);
                }
                break;
#endif  // GH_NO_FETCH

#ifndef GH_NO_UPLOAD
            case gh::CMD::Upload:  // name == path, value == size
                if (!_upl_p) {
                    _upl_p = new Uploader(client, _id, _safe_upl);
                    if (_upl_p) {
                        if (!_upl_p->begin(name, value)) GHDELPTR(_upl_p);
                    } else {
                        Uploader::sendError(client, _id, gh::Error::Memory);
                    }
                } else {
                    Uploader::sendError(client, _id, gh::Error::Busy);
                }
                break;

            case gh::CMD::UploadChunk:  // name == next/last/crc, value == data64
                if (_upl_p && _upl_p->client == client) {
                    _upl_p->process(nameHash, value);

                    if (_upl_p->last() && !_upl_p->hasError()) {
                        if (_upload_cb) _upload_cb(_upl_p->path);
                    }
                    if (_upl_p->last() || _upl_p->hasError()) GHDELPTR(_upl_p);
                } else {
                    Uploader::sendError(client, _id, gh::Error::WrongClient);
                }
                break;
#endif  // GH_NO_UPLOAD
#endif  // GH_NO_FS

#ifndef GH_NO_OTA
            case gh::CMD::Ota:  // name == flash/fs
                if (!_ota_p) {
                    _ota_p = new Updater(client, &_reason, _id);
                    if (_ota_p) {
                        if (!_ota_p->begin(name)) GHDELPTR(_ota_p);
                    } else {
                        Updater::sendError(client, _id, gh::Error::Memory);
                    }
                } else {
                    Updater::sendError(client, _id, gh::Error::Busy);
                }
                break;

            case gh::CMD::OtaChunk:  // name == next/last, value == data
                if (_ota_p && _ota_p->client == client) {
                    _ota_p->process(nameHash, value);
                    if (_ota_p->hasError() || _ota_p->last()) GHDELPTR(_ota_p);
                } else {
                    Updater::sendError(client, _id, gh::Error::WrongClient);
                }
                break;
#endif  // GH_NO_OTA

#ifndef GH_NO_OTA_URL
            case gh::CMD::OtaUrl:  // name == flash/fs, value == url
                if (!_otaurl_p) {
                    _otaurl_p = new OtaUrl(nameHash, value, client, _id);
                    if (!_otaurl_p) {
                        OtaUrl::sendError(client, _id, gh::Error::Memory);
                    }
                } else {
                    OtaUrl::sendError(client, _id, gh::Error::Busy);
                }
                break;
#endif  // GH_NO_OTA_URL

#endif  // GH_ESP_BUILD
            default:
                break;
        }
    }
};

}  // namespace ghc