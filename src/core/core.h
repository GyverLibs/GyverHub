#pragma once

#include <Arduino.h>
#include <StringUtils.h>

#include "bridge.h"
#include "build.h"
#include "callbacks.h"
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
#include "ui/data.h"
#include "ui/info.h"
#include "ui/location.h"

#ifdef GH_ESP_BUILD

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif  // ESP8266

#ifdef ESP32
#include <WiFi.h>
#endif  // ESP32

#include "transfer/fetcher.h"

#ifndef GH_NO_OTA
#include "transfer/updater.h"
#endif

#ifndef GH_NO_OTA_URL
#include "transfer/ota_url.h"
#endif

#ifndef GH_NO_FS
#ifndef GH_NO_UPLOAD
#include "transfer/uploader.h"
#endif
#endif  // GH_NO_FS

#ifndef GH_NO_HTTP
#include "bridges/esp/sync/http.h"
#endif
#ifndef GH_NO_MQTT
#include "bridges/esp/sync/mqtt.h"
#endif
#ifndef GH_NO_WS
#include "bridges/esp/sync/ws.h"
#endif

#endif  // GH_ESP_BUILD

namespace ghc {

// ========================== CLASS ==========================
class HubCore {
   public:
    // добавить мост подключения. false если нет места
    bool addBridge(gh::Bridge* bridge) {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (!bridges[i]) {
                bridges[i] = bridge;
                return 1;
            }
        }
        return 0;
    }

    // отправить текст клиенту
    void _send(GHTREF text, gh::Client* client = nullptr) {
        if (!text.length()) return;
        if (client) client->_sent = true;

        gh::BridgeData data(text);
        if (client && client->bridge) {
            data.broadcast = false;
            if (client->connection == gh::Connection::MQTT) {
                data.topic = _topicSend(client->id);
            }
            if (client->bridge->canSend()) client->bridge->send(data);
        } else {
            data.broadcast = true;
            data.topic = _topicSend();
            for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
                if (bridges[i] && bridges[i]->getFocus() && bridges[i]->canSend()) {
                    bridges[i]->send(data);
                }
            }
        }
    }

    // ==========================================================================
    // ================================== HOOKS =================================
    // ==========================================================================
    static void parseHook(void* hub, gh::Bridge& bridge, GHTREF url, GHTREF data) {
        ((HubCore*)hub)->_parse(bridge, url, data);
    }
    static void sendHook(void* hub, GHTREF text, gh::Client* client) {
        ((HubCore*)hub)->_send(text, client);
    }

#ifdef GH_ESP_BUILD
    static void fetchHook(void* hubptr, gh::Fetcher* fetcher) {
        if (((HubCore*)hubptr)->_fetch_cb) ((HubCore*)hubptr)->_fetch_cb(*fetcher);
    }
    static void uploadHook(void* hubptr, String& path) {
        if (((HubCore*)hubptr)->_upload_cb) ((HubCore*)hubptr)->_upload_cb(path);
    }
    static bool requestHook(void* hubptr, gh::Request* request) {
#ifndef GH_NO_REQUEST
        if (((HubCore*)hubptr)->_request_cb) return ((HubCore*)hubptr)->_request_cb(*request);
#endif
        return 1;
    }
#endif  // GH_ESP_BUILD

    // VARS
    uint8_t menu = 0;  // выбранный пункт меню
    String net = "";
    String name = "";
    String icon = "";
    String version = "";
    uint32_t id = 0;
    gh::Bridge* bridges[GH_BRIDGE_AMOUNT] = {nullptr};

// модули
#ifndef GH_NO_MODULES
    Modules modules;
#endif

#if defined(GH_ESP_BUILD) && !defined(GH_NO_MQTT)
    HubMQTT mqtt;
#endif

    // ==========================================================================
    // ================================= PRIVATE ================================
    // ==========================================================================
   protected:
    uint32_t _pin = 0;

    uint16_t _bufsize = 1000;
    bool _running_f = false;
    bool _build_busy = false;  // (билдер запущен) запрещает вызывать функции, которые вызывают билдер
    bool _allow_send = true;   // разрешает отправку sendXxx и класс Update только вне билдера + в Set билдере
    bool _autoGet_f = true;
    bool _broadcast = true;

    BuildCallback _build_cb = nullptr;
    PingCallback _ping_cb = nullptr;
    CliCallback _cli_cb = nullptr;
    DataCallback _data_cb = nullptr;
    InfoCallback _info_cb = nullptr;
    UnixCallback _unix_cb = nullptr;
    LocationCallback _loc_cb = nullptr;
#ifndef GH_NO_REQUEST
    RequestCallback _request_cb = nullptr;
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
#ifdef GH_ESP_BUILD
#ifndef GH_NO_HTTP
#ifndef GH_NO_MODULES
        http.setup(this, parseHook, fetchHook, requestHook, uploadHook, &_safe_upl, &modules, &_reason);
#else
        http.setup(this, parseHook, fetchHook, requestHook, uploadHook, &_safe_upl, nullptr, &_reason);
#endif
        addBridge(&http);
#endif  // GH_NO_HTTP

#ifndef GH_NO_WS
        ws.setup(this, parseHook);
        addBridge(&ws);
#endif
#ifndef GH_NO_MQTT
        mqtt.setup(this, parseHook, &id, &net);
        addBridge(&mqtt);
#endif
#endif
    }

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
        p.addString(Tag::prefix, net);
        p.addString(Tag::icon, icon);
        p.addInt(Tag::PIN, hash);
        p.addString(Tag::version, version);
        p.addString(Tag::platform, F(GH_PLATFORM));
        p.addInt(Tag::max_upload, GH_UPL_CHUNK_SIZE);
        p.addInt(Tag::api_v, GH_API_VERSION);

        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i] && bridges[i]->connection == gh::Connection::UDP) {
                p.addInt(Tag::udp_port, bridges[i]->getPort());
                break;
            }
        }
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i] && bridges[i]->connection == gh::Connection::WS) {
                p.addInt(Tag::ws_port, bridges[i]->getPort());
                break;
            }
        }
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i] && bridges[i]->connection == gh::Connection::HTTP) {
                p.addInt(Tag::http_port, bridges[i]->getPort());
                p.addInt(Tag::http_transfer, bridges[i]->hasTransfer());
                break;
            }
        }

#ifdef ATOMIC_FS_UPDATE
        p.addString(Tag::ota_type, F("gz"));
#else
        p.addString(Tag::ota_type, F("bin"));
#endif
#ifndef GH_NO_MODULES
        p.addInt(Tag::modules, modules.mods);
#else
        p.addInt(Tag::modules, 0);
#endif
        p.endPacket();
        p.send();
    }
    void _answerAck(GHTREF name, gh::Client& client, GHTREF packet_id) {
        Packet p(50, &client);
        p.beginPacket(id);
        p.addString(Tag::type, Tag::ack);
        p.addString(Tag::name, name);
        if (packet_id.valid()) p.addString(Tag::packet_id, packet_id);
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
            if (bridges[i] && bridges[i]->connection == gh::Connection::MQTT) {
                bridges[i]->send(data);
            }
        }
    }
    // net/hub
    String _topicSend() {
        String topic(net);
        topic += F("/hub");
        return topic;
    }
    // net/hub/client_id/id
    String _topicSend(uint32_t client_id) {
        String topic = _topicSend();
        topic += '/';
        _addID(topic, client_id);
        topic += '/';
        _addID(topic, id);
        return topic;
    }
    // net/hub/id/get/name
    String _topicGet(GHTREF name) {
        String topic(net);
        topic += F("/hub/");
        _addID(topic, id);
        topic += F("/get/");
        name.addString(topic);
        return topic;
    }
    // net/hub/id/status
    String _topicStatus() {
        String topic(net);
        topic += F("/hub/");
        _addID(topic, id);
        topic += F("/status");
        return topic;
    }
    // net/device_id/id/data/name
    String _topicData(uint32_t device_id, GHTREF name) {
        String topic(net);
        topic.reserve(50);
        topic += '/';
        _addID(topic, device_id);
        topic += '/';
        _addID(topic, id);
        topic += F("/data/");
        name.addString(topic);
        return topic;
    }
    // add id to string
    void _addID(String& s, uint32_t& id) {
        su::Value(id, HEX).addString(s);
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
            return b._namer.isFound();
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
    bool _request(gh::Client& client, bool broadcast, gh::CMD cmd, GHTREF name = GHTXT(), GHTREF value = GHTXT()) {
        if (!_request_cb) return 1;
        gh::Request req(client, broadcast, cmd, name, value);
        return _request_cb(req);
    }
#endif

    void _discover(gh::Bridge& bridge, GHTREF id, gh::CMD cmd) {
        gh::Client client(this, sendHook, &bridge, id.toInt32HEX());
#ifndef GH_NO_REQUEST
        if (_request(client, true, cmd)) {
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

    // net/device_id/client_id/CMD/name + value
    // net/device_id/client_id-packet_id/CMD/name + value
    void _parse(gh::Bridge& bridge, GHTREF url, GHTREF value) {
        if (url == net) return _discover(bridge, value, gh::CMD::Search);

        su::Text list[5];
        uint8_t len = url.split(list, 5, '/');
        if (list[0] != net) return;  // wrong net

        uint32_t device_id = list[1].toInt32HEX();
        if (device_id != id && !(_broadcast && device_id == GH_BROAD_ID)) return;  // wrong id
        if (len == 2) return _discover(bridge, value, gh::CMD::Discover);
        if (len == 3) return;

        gh::CMD cmd = getCMD(list[3]);
        if (cmd == gh::CMD::Unknown) return;

        su::Text packet_id;
        su::Text& client_id = list[2];
        int dash = client_id.indexOf('-');  // <client_id>-<packet_id>
        if (dash > 0) {
            packet_id = client_id.substring(dash + 1);
            client_id = client_id.substring(0, dash);
        }

        const su::Text& name = list[4];
        gh::Client client(this, sendHook, &bridge, client_id.toInt32HEX());

#ifndef GH_NO_REQUEST
        if (!_request(client, device_id == GH_BROAD_ID, cmd, name, value)) {
            return _answerError(Tag::error, gh::Error::Forbidden, client);
        }
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
                if (_ping_cb) _ping_cb(client);
                if (!client._sent) _answerCmd(Tag::OK, client);
                break;

            case gh::CMD::Unfocus:
                bridge.clearFocus();
                _answerCmd(Tag::OK, client);
                break;

            case gh::CMD::Location:
                if (_loc_cb) _loc_cb(gh::Location(name, value, client));
                break;

            case gh::CMD::Info:
#ifdef GH_NO_INFO
                _answerCmd(Tag::OK, client);
#else
                _answerInfo(client);
#endif
                break;

            case gh::CMD::Data:
                if (_data_cb) _data_cb(gh::Data{name, value, client});
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
                else _answerAck(name, client, packet_id);

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
                gh::FS.remove(name.toString());
                _answerFiles(client);
                break;

            case gh::CMD::Rename:
                gh::FS.rename(name.toString(), value.toString());
                _answerFiles(client);
                break;

            case gh::CMD::Create: {  // name == path
                gh::FS.mkdir(value.toString());
                File f = gh::FS.openWrite(name.toString());
                f.close();
                _answerFiles(client);
            } break;

            case gh::CMD::FsAbort:
                _answerCmd(Tag::OK, client);
                switch (name.hash()) {  // name == type
#ifndef GH_NO_UPLOAD
                    case su::SH("upload"):
                        if (_upl_p && _upl_p->client == client) {
                            _upl_p->abort();
                            GHDELPTR(_upl_p);
                        }
                        break;
#endif  // GH_NO_UPLOAD
#ifndef GH_NO_FETCH
                    case su::SH("fetch"):
                    case su::SH("fetch_file"):
                        if (_fet_p && _fet_p->client == client) {
                            _fet_p->abort();
                            GHDELPTR(_fet_p);
                        }
                        break;
#endif  // GH_NO_FETCH
#ifndef GH_NO_OTA
                    case su::SH("ota"):
                        if (_ota_p && _ota_p->client == client) {
                            _ota_p->abort();
                            GHDELPTR(_ota_p);
                        }
                        break;
#endif  // GH_NO_OTA
                    case su::SH("all"):
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
                    _fet_p = new gh::Fetcher(client, fetchHook, name, id);
                    if (_fet_p) {
                        if (!_fet_p->begin()) GHDELPTR(_fet_p);
                    } else {
                        gh::Fetcher::sendError(client, id, gh::Error::Memory);
                    }
                } else {
                    gh::Fetcher::sendError(client, id, gh::Error::Busy);
                }
                break;

            case gh::CMD::FetchNext:
                if (_fet_p && _fet_p->client == client) {
                    if (_fet_p->next()) GHDELPTR(_fet_p);  // last
                } else {
                    gh::Fetcher::sendError(client, id, gh::Error::WrongClient);
                }
                break;
#endif  // GH_NO_FETCH

#ifndef GH_NO_UPLOAD
            case gh::CMD::Upload:  // name == path, value == size
                if (!_upl_p) {
                    _upl_p = new Uploader(client, id, _safe_upl);
                    if (_upl_p) {
                        if (!_upl_p->begin(name, value)) GHDELPTR(_upl_p);
                    } else {
                        Uploader::sendError(client, id, gh::Error::Memory);
                    }
                } else {
                    Uploader::sendError(client, id, gh::Error::Busy);
                }
                break;

            case gh::CMD::UploadChunk:  // name == next/last/crc, value == data64
                if (_upl_p && _upl_p->client == client) {
                    _upl_p->process(name.hash(), value);

                    if (_upl_p->last() && !_upl_p->hasError()) {
                        if (_upload_cb) _upload_cb(_upl_p->path);
                    }
                    if (_upl_p->last() || _upl_p->hasError()) GHDELPTR(_upl_p);
                } else {
                    Uploader::sendError(client, id, gh::Error::WrongClient);
                }
                break;
#endif  // GH_NO_UPLOAD
#endif  // GH_NO_FS

#ifndef GH_NO_OTA
            case gh::CMD::Ota:  // name == flash/fs
                if (!_ota_p) {
                    _ota_p = new Updater(client, &_reason, id);
                    if (_ota_p) {
                        if (!_ota_p->begin(name)) GHDELPTR(_ota_p);
                    } else {
                        Updater::sendError(client, id, gh::Error::Memory);
                    }
                } else {
                    Updater::sendError(client, id, gh::Error::Busy);
                }
                break;

            case gh::CMD::OtaChunk:  // name == next/last, value == data
                if (_ota_p && _ota_p->client == client) {
                    _ota_p->process(name.hash(), value);
                    if (_ota_p->hasError() || _ota_p->last()) GHDELPTR(_ota_p);
                } else {
                    Updater::sendError(client, id, gh::Error::WrongClient);
                }
                break;
#endif  // GH_NO_OTA

#ifndef GH_NO_OTA_URL
            case gh::CMD::OtaUrl:  // name == flash/fs, value == url
                if (!_otaurl_p) {
                    _otaurl_p = new OtaUrl(name.hash(), value, client, id);
                    if (!_otaurl_p) {
                        OtaUrl::sendError(client, id, gh::Error::Memory);
                    }
                } else {
                    OtaUrl::sendError(client, id, gh::Error::Busy);
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