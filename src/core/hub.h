#pragma once

#include <Arduino.h>
#include <StringUtils.h>

#include "core.h"
#include "hub_macro.hpp"
#include "ui/update_inline.h"

class GyverHub : public ghc::HubCore {
   public:
    // настроить префикс, название и иконку. Опционально задать свой ID устройства вида 0xXXXXXX (для esp он генерируется автоматически)
    GyverHub(const String& net = "", const String& name = "", const String& icon = "", uint32_t id = 0) {
        config(net, name, icon, id);
        _init();
    }

    // ========================= SYSTEM =========================

    // настроить префикс, название и иконку. Опционально задать свой ID устройства вида 0xXXXXXX (для esp он генерируется автоматически)
    void config(const String& net, const String& name = "", const String& icon = "", uint32_t id = 0) {
        this->net = net;
        this->name = name;
        this->icon = icon;

        if (!id) {
#ifdef GH_ESP_BUILD
            uint8_t mac[6];
            WiFi.macAddress(mac);
            this->id = *((uint32_t*)(mac + 2));
#else
            this->id = 1;
#endif
        } else {
            this->id = id;
        }
    }

    // установить версию прошивки для отображения в Info
    void setVersion(const String& v) {
        version = v;
    }

    // установить размер буфера строки для сборки интерфейса (умолч. 1000)
    void setBufferSize(uint16_t size) {
        _bufsize = size;
    }

    // разрешить принимать широковещательные запросы (умолч. true)
    void allowBroadcast(bool allow) {
        _broadcast = allow;
    }

    // установить пин-код для открытия устройства (значение больше 1000, не может начинаться с 000..)
    void setPIN(uint32_t pin) {
        _pin = pin;
    }

    // прочитать пин-код
    uint32_t getPIN() {
        return _pin;
    }

    // запустить систему и fs
    void begin(bool startFS = true) {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i]) bridges[i]->begin();
        }

#ifndef GH_NO_FS
        if (startFS) gh::FS.begin();
#endif
        _running_f = true;
    }

    // остановить
    void end() {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i]) bridges[i]->end();
        }
        _running_f = false;
    }

    // ========================== STATUS ==========================

    // вернёт true, если система запущена
    bool running() {
        return _running_f;
    }

    // true - интерфейс устройства сейчас открыт на сайте или в приложении
    bool focused() {
        for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
            if (bridges[i] && bridges[i]->getFocus()) return 1;
        }
        return 0;
    }

    // доступна ли сейчас отправка (функции updateXxx и sendXxx)
    bool canSend() {
        return _allow_send;
    }

    // ========================= ATTACH =========================
    void attach(void (*h)()) {}
    // подключить функцию-сборщик интерфейса вида f(gh::Builder& builder)
    void onBuild(ghc::BuildCallback callback) {
        _build_cb = callback;
    }

    // подключить обработчик запроса клиента вида f(gh::Request& request)
    void onRequest(ghc::RequestCallback callback) {
#ifndef GH_NO_REQUEST
        _request_cb = callback;
#endif
    }

    // подключить функцию-сборщик инфо вида f(gh::Info& info)
    void onInfo(ghc::InfoCallback callback) {
        _info_cb = callback;
    }

    // подключить обработчик входящих сообщений с веб-консоли вида f(String str)
    void onCLI(ghc::CliCallback callback) {
        _cli_cb = callback;
    }

    // подключить обработчик получения Unix времени с клиента вида f(uint32_t stamp)
    void onUnix(ghc::UnixCallback callback) {
        _unix_cb = callback;
    }

    // подключить обработчик получения геолокации с клиента вида f(gh::Location location)
    void onLocation(ghc::LocationCallback callback) {
        _loc_cb = callback;
    }

    // подключить обработчик получения данных вида f(gh::Data data)
    void onData(ghc::DataCallback callback) {
        _data_cb = callback;
    }

    // подключить обработчик пинга вида f(gh::Client& client)
    void onPing(ghc::PingCallback callback) {
        _ping_cb = callback;
    }

#ifdef GH_ESP_BUILD
    // подключить обработчик скачивания файлов вида f(gh::Fetcher& fetcher)
    void onFetch(ghc::FetchCallback callback) {
        _fetch_cb = callback;
    }

    // подключить функцию-обработчик перезагрузки вида f(gh::Reboot res). Будет вызвана перед перезагрузкой
    void onReboot(ghc::RebootCallback callback) {
        _reboot_cb = callback;
    }

    // подключить обработчик загрузки файлов вида f(String& path). Будет вызван при сохранении файла
    void onUpload(ghc::UploadCallback callback) {
        _upload_cb = callback;
    }

    // безопасная загрузка файлов (загружать во временный файл) (умолч. включен, true)
    void safeUpload(bool state) {
        _safe_upl = state;
    }
#endif  // GH_ESP_BUILD

    // ========================= SEND =========================

    // запросить геолокацию (придёт в onLocation)
    void requestLocation(gh::Client* client, bool highAccuracy = false) {
        if (!focused() || !_allow_send || !client->bridge) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::location);
        if (highAccuracy) p.addBool(ghc::Tag::high_accuracy, 1);
        p.endPacket();
        _send(p, client);
    }

    // отправить текст в веб-консоль. Опционально цвет
    void sendCLI(GHTREF str, gh::Colors col = gh::Colors::Default, gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::print);
        p.addStringEsc(ghc::Tag::text, str);
        p.addString(ghc::Tag::color, su::Value((uint32_t)col, HEX));
        p.endPacket();
        _send(p, client);
    }

    // обновить панель управления в приложении
    void sendRefresh(gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        _sendRefresh(client);
    }

    // выполнить js код
    void sendScript(GHTREF script, gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::script);
        p.addStringEsc(ghc::Tag::script, script);
        p.endPacket();
        _send(p, client);
    }

    // ========================= ACTION =========================

    // отправить действие (обновить файл, вызвать Confirm/Prompt)
    void sendAction(GHTREF name, gh::Client* client = nullptr) {
        if (!focused() || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::update);
        p.beginObj(ghc::Tag::updates);
        p.addText(name, F("{\"action\":1}"));
        p.endObj();
        p.endPacket();
        _send(p, client);
    }

    // ======================= UPDATE INL ========================

    // обновить виджет. Указать имя виджета (или список), клиента (опционально)
    gh::UpdateInline update(GHTREF name, gh::Client* client = nullptr) {
        return gh::UpdateInline(canSend(), id, name, client ? *client : gh::Client(this, sendHook));
    }

    // ========================= UPDATE =========================

    // отправить value update на имя виджета int/string/bool
    void sendUpdate(GHTREF name, const su::Value& value, gh::Client* client = nullptr) {
        if (!focused() || !canSend()) return;
        _sendUpdate(name, value, client);
    }

    // отправить value update на имя виджета float
    void sendUpdate(GHTREF name, double value, uint8_t dec, gh::Client* client = nullptr) {
        sendUpdate(su::Value(value, dec));
    }

    // отправить value update по имени компонента (значение будет прочитано в build). Нельзя вызывать из build. Имена можно передать списком через ;
    void sendUpdate(GHTREF name, gh::Client* client = nullptr) {
        if (!_build_cb || !focused() || _build_busy) return;
        gh::Client read_client;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::update);
        p.beginObj(ghc::Tag::updates);
        for (su::TextParser n(name, ';'); n.parse();) {
            p.addKey(n);
            p.s += F("{\"value\":\"");
            p.reserve(p.length() + 30);
            _readBuild(p, read_client, n);
            p.s += F("\"},");
        }
        p.endObj();
        p.endPacket();
        _send(p, client);
    }

    // ========================= DATA =========================

    // отправить устройству device_id данные с именем name и значением value
    void sendData(uint32_t device_id, gh::Bridge* bridge, GHTREF name, const su::Value& value) {
        if (!name.valid() || !value.valid() || !bridge->canSend()) return;
        if (bridge->connection == gh::Connection::MQTT) {
            gh::BridgeData data(value);
            data.topic = _topicData(device_id, name);
            bridge->send(data);
        } else {
            String url(_topicData(device_id, name));
            url += '=';
            value.addString(url);
            gh::BridgeData data(url);
            bridge->send(data);
        }
    }

    // отправить всем устройствам данные с именем name и значением value
    void sendDataAll(gh::Bridge* bridge, GHTREF name, const su::Value& value) {
        sendData(GH_BROAD_ID, bridge, name, value);
    }

    // отправить устройству device_id данные с именем name и значением value
    void sendData(uint32_t device_id, gh::Bridge* bridge, GHTREF name, double value, uint8_t dec) {
        sendData(device_id, bridge, name, su::Value(value, dec));
    }

    // отправить всем устройствам данные с именем name и значением value
    void sendDataAll(gh::Bridge* bridge, GHTREF name, double value, uint8_t dec) {
        sendData(GH_BROAD_ID, bridge, name, value, dec);
    }

    // ========================= MQTT =========================

    // автоматически отправлять новое состояние на get-топик при изменении через set (умолч. false)
    void sendGetAuto(bool state) {
        _autoGet_f = state;
    }

    // отправить имя-значение на get-топик (MQTT) int/string/bool
    void sendGet(GHTREF name, const su::Value& value) {
        _sendGet(name, value);
    }

    // отправить имя-значение на get-топик (MQTT) float
    void sendGet(GHTREF name, double value, uint8_t dec) {
        sendGet(name, su::Value(value, dec));
    }

    // отправить значение по имени компонента на get-топик (MQTT) (значение будет прочитано в build). Имена можно передать списком через ;
    void sendGet(GHTREF name) {
        if (!_running_f || !_build_cb || _build_busy) return;
        gh::Client client;
        ghc::Packet p(30);
        for (su::TextParser n(name, ';'); n.parse();) {
            bool found = _readBuild(p, client, n);
            if (found) _sendGet(n, p);
        }
    }

    // отправить MQTT LWT команду на включение/выключение
    void sendStatus(bool status) {
        String topic = _topicStatus();
        _sendMQTT(topic, status ? F("online") : F("offline"));
    }

    // топик статуса для отправки
    String topicStatus() {
        String t(net);
        t += F("/hub/");
        _addID(t, id);
        t += F("/status");
        return t;
    }

    // общий топик для подписки
    String topicDiscover() {
        return net;
    }

    // топик устройства для подписки
    String topicHub() {
        String t(net);
        t += '/';
        _addID(t, id);
        t += "/#";
        return t;
    }

    // ========================= NOTIFY ==========================

    // отправить пуш уведомление
    void sendPush(GHTREF text, gh::Client* client = nullptr) {
        if (!_running_f || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::push);
        p.addStringEsc(ghc::Tag::text, text);
        p.endPacket();
        _send(p, client);
    }

    // отправить всплывающее уведомление
    void sendNotice(GHTREF text, gh::Colors col = gh::Colors::Default, gh::Client* client = nullptr) {
        if (!_running_f || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::notice);
        p.addStringEsc(ghc::Tag::text, text);
        p.addString(ghc::Tag::color, su::Value((uint32_t)col, HEX));
        p.endPacket();
        _send(p, client);
    }

    // показать окно с ошибкой
    void sendAlert(GHTREF text, gh::Client* client = nullptr) {
        if (!_running_f || !_allow_send) return;
        ghc::Packet p(50);
        p.beginPacket(id, client);
        p.addString(ghc::Tag::type, ghc::Tag::alert);
        p.addStringEsc(ghc::Tag::text, text);
        p.endPacket();
        _send(p, client);
    }

    // ========================= GETTER =========================

    // получить полный JSON пакет панели управления. Флаг enclose true - обернуть в результат в []
    String getUI(bool enclose = true) {
        if (_build_busy) return String();
        gh::Client client;
        ghc::Packet p;
        if (enclose) p.beginArr();
        _uiBuild(p, client);
        if (enclose) p.endArr();
        p.end();
        return p.s;
    }

    // получить JSON объект {имя:значение, ...} виджетов (из билдера)
    String getValues() {
        if (_build_busy) return String();
        ghc::Packet p(50);
        p.beginObj();
        if (_build_cb) {
            _build_busy = true;
            _allow_send = false;
            gh::Client client;
            gh::Build build(gh::Action::Get, client);
            gh::Builder b(build, &p);
            _build_cb(b);
            _allow_send = true;
            _build_busy = false;
        }
        p.endObj();
        p.end();
        return p.s;
    }

    // получить значение компонента по имени (из билдера)
    String getValue(GHTREF name) {
        if (_build_busy) return String();
        gh::Client client;
        ghc::Packet p;
        _readBuild(p, client, name);
        return p.s;
    }

    // ========================= TICK =========================

    // тикер, вызывать в loop
    bool tick() {
#ifdef GH_ESP_BUILD

#ifndef GH_NO_FS
#ifndef GH_NO_FETCH
        if (_fet_p && _fet_p->timeout()) GHDELPTR(_fet_p);
#endif
#ifndef GH_NO_UPLOAD
        if (_upl_p && _upl_p->timeout()) GHDELPTR(_upl_p);
#endif
#endif  // GH_NO_FS

#ifndef GH_NO_OTA
        if (_ota_p && _ota_p->timeout()) GHDELPTR(_ota_p);
#endif
        if (_reason != gh::Reboot::None) {
            if (_reboot_cb) _reboot_cb(_reason);
            delay(GH_REBOOT_DELAY);
            ESP.restart();
        }

#ifndef GH_NO_OTA_URL
        if (_otaurl_p) {
            if (_otaurl_p->update()) {
                _reason = gh::Reboot::OtaUrl;
            }
            GHDELPTR(_otaurl_p);
        }
#endif

#endif  // GH_ESP_BUILD

        if (_running_f) {
            for (uint8_t i = 0; i < GH_BRIDGE_AMOUNT; i++) {
                if (bridges[i] && bridges[i]->canParse()) bridges[i]->tick();
            }
            return 1;
        }
        return 0;
    }

   private:
};