#ifndef _GyverHUB_h
#define _GyverHUB_h

#include <Arduino.h>

#include "builder.h"
#include "canvas.h"
#include "config.hpp"
#include "macro.hpp"
#include "stream.h"
#include "utils/build.h"
#include "utils/cmd_p.h"
#include "utils/color.h"
#include "utils/datatypes.h"
#include "utils/flags.h"
#include "utils/log.h"
#include "utils/misc.h"
#include "utils/modules.h"
#include "utils/stats.h"
#include "utils/stats_p.h"
#include "utils/timer.h"

#ifdef GH_ESP_BUILD
#include <FS.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#ifndef GH_NO_OTA
#ifndef GH_NO_OTA_URL
#include <ESP8266httpUpdate.h>
#endif
#endif
#endif

#ifdef ESP32
#include <WiFi.h>
#ifndef GH_NO_OTA
#include <Update.h>
#ifndef GH_NO_OTA_URL
#include <HTTPUpdate.h>
#endif
#endif
#endif

#ifndef GH_NO_FS
#if (GH_FS == LittleFS)
#include <LittleFS.h>
#elif (GH_FS == SPIFFS)
#include <SPIFFS.h>
#endif
#endif

#ifdef GH_ASYNC
#include "async/http.h"
#include "async/mqtt.h"
#include "async/ws.h"
#else
#include "sync/http.h"
#include "sync/mqtt.h"
#include "sync/ws.h"
#endif

#endif

// ========================== CLASS ==========================
#ifdef GH_ESP_BUILD
class GyverHub : public HubBuilder, public HubStream, public HubHTTP, public HubMQTT, public HubWS {
#else
class GyverHub : public HubBuilder, public HubStream {
#endif
   public:
    // ========================== CONSTRUCT ==========================

    // настроить префикс, название и иконку. Опционально задать свой ID устройства (для esp он генерируется автоматически)
    GyverHub(const char* prefix = "", const char* name = "", const char* icon = "", uint32_t id = 0) {
        config(prefix, name, icon, id);
    }

    // настроить префикс, название и иконку. Опционально задать свой ID устройства (для esp он генерируется автоматически)
    void config(const char* nprefix, const char* nname, const char* nicon, uint32_t nid = 0) {
        prefix = nprefix;
        name = nname;
        icon = nicon;
#ifdef GH_ESP_BUILD
        if (nid) ultoa((nid <= 0xfffff) ? (nid + 0xfffff) : nid, id, HEX);
        else {
            uint8_t mac[6];
            WiFi.macAddress(mac);
            ultoa(*((uint32_t*)(mac + 2)), id, HEX);
        }
#else
        ultoa((nid <= 0x100000) ? (nid + 0x100000) : nid, id, HEX);
#endif
    }

    // ========================== SETUP ==========================

    // запустить
    void begin() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
        beginWS();
        beginHTTP();
#endif
#ifndef GH_NO_MQTT
        beginMQTT();
#endif
#ifndef GH_NO_FS
#ifdef ESP8266
        fs_mounted = GH_FS.begin();
#else
        fs_mounted = GH_FS.begin(true);
#endif
#endif
#endif
        running_f = true;
        sendEvent(GH_START, GH_SYSTEM);
    }

    // остановить
    void end() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
        endWS();
        endHTTP();
#endif
#ifndef GH_NO_MQTT
        endMQTT();
#endif
#endif
        running_f = false;
        sendEvent(GH_STOP, GH_SYSTEM);
    }

    // установить версию прошивки для отображения в Info и OTA
    void setVersion(const char* v) {
        version = v;
    }

    // установить размер буфера строки для сборки интерфейса при ручной отправке
    // 0 - интерфейс будет собран и отправлен цельной строкой
    // >0 - пакет будет отправляться частями
    void setBufferSize(uint16_t size) {
        buf_size = size;
    }

    // включение/отключение системных модулей
    GHmodule modules;

    // ========================== PIN ==========================

    // установить пин-код для открытия устройства (значение больше 1000, не может начинаться с 000..)
    void setPIN(uint32_t npin) {
        PIN = npin;
    }

    // прочитать пин-код
    uint32_t getPIN() {
        return PIN;
    }

    // ========================= ATTACH =========================

    // подключить функцию-сборщик интерфейса
    void onBuild(void (*handler)()) {
        build_cb = *handler;
    }

    // подключить функцию-обработчик запроса при ручном соединении
    void onManual(void (*handler)(String& s, GHconn_t from, bool broadcast)) {
        manual_cb = *handler;
    }

    // ========================= INFO =========================

    // подключить функцию-сборщик инфо
    void onInfo(void (*handler)(GHinfo_t info)) {
        info_cb = *handler;
    }

    // добавить поле в info
    void addInfo(const String& label, const String& text) {
        if (sptr) {
            *sptr += '\"';
            GH_addEsc(sptr, label.c_str());//*sptr += label;
            *sptr += F("\":\"");
            GH_addEsc(sptr, text.c_str());//*sptr += text;
            *sptr += F("\",");
        }
    }

    // ========================= CLI =========================

    // подключить обработчик входящих сообщений с веб-консоли
    void onCLI(void (*handler)(String& s)) {
        cli_cb = *handler;
    }

    // отправить текст в веб-консоль. Опционально цвет
    void print(const String& str, uint32_t color = GH_DEFAULT) {
        if (!focused()) return;
        String answ;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("print"));
        _jsStr(answ, F("text"), str);
        _jsVal(answ, F("color"), color, true);
        _jsEnd(answ);
        _send(answ);
    }

    // ========================== STATUS ==========================

    // подключить обработчик изменения статуса
    void onEvent(void (*handler)(GHevent_t state, GHconn_t from)) {
        event_cb = *handler;
    }

    // отправить event для отладки
    void sendEvent(GHevent_t state, GHconn_t from) {
        if (event_cb) event_cb(state, from);
    }

    // вернёт true, если система запущена
    bool running() {
        return running_f;
    }

    // подключить функцию-обработчик перезагрузки. Будет вызвана перед перезагрузкой
    void onReboot(void (*handler)(GHreason_t r)) {
#ifdef GH_ESP_BUILD
        reboot_cb = *handler;
#endif
    }

    // получить свойства текущего билда. Вызывать внутри обработчика
    GHbuild getBuild() {
        return bptr ? *bptr : GHbuild();
    }

    // true - интерфейс устройства сейчас открыт на сайте или в приложении
    bool focused() {
        if (!running_f) return 0;
        for (uint8_t i = 0; i < GH_CONN_AMOUNT; i++) {
            if (focus_arr[i]) return 1;
        }
        return 0;
    }

    // проверить фокус по указанному типу связи
    bool focused(GHconn_t from) {
        return focus_arr[from];
    }

    // обновить веб-интерфейс. Вызывать внутри обработчика build
    void refresh() {
        refresh_f = true;
    }

    // true - если билдер вызван для set или read операций
    bool buildRead() {
        return (bptr && (bptr->type == GH_BUILD_ACTION || bptr->type == GH_BUILD_READ));
    }

    // true - если билдер вызван для запроса компонентов (при загрузке панели управления)
    bool buildUI() {
        return (bptr && (bptr->type == GH_BUILD_UI || bptr->type == GH_BUILD_COUNT));
    }

    // получить текущее действие для ручной обработки значений
    const GHaction& action() const {
        return bptr->action;
    }

    // подключить обработчик запроса клиента
    void onRequest(bool (*handler)(GHbuild build)) {
        req_cb = *handler;
    }

    // ========================= FETCH ==========================

    // подключить обработчик скачивания
    void onFetch(void (*handler)(String& path, bool start)) {
        fetch_cb = *handler;
    }

    // отправить файл (вызывать в обработчике onFetch)
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
    void fetchFile(const char* path) {
        file_d = GH_FS.open(path, "r");
    }
#endif
#endif

    // отправить сырые данные (вызывать в обработчике onFetch)
    void fetchBytes(uint8_t* bytes, uint32_t size) {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
        file_b = bytes;
        file_b_size = size;
#endif
#endif
    }

    // ========================= DATA ==========================
    // подключить обработчик данных (см. GyverHub.js API)
    void onData(void (*handler)(const char* name, const char* value)) {
        data_cb = *handler;
    }

    // ответить клиенту. Вызывать в обработчике onData (см. GyverHub.js API)
    void answer(const String& data) {
        String answ;
        _datasend(answ, data);
        _answer(answ);
    }

    // отправить сырые данные вручную (см. GyverHub.js API)
    void send(const String& data) {
        String answ;
        _datasend(answ, data);
        _send(answ);
    }

    // ========================= NOTIF ==========================
    // отправить пуш уведомление
    void sendPush(const String& text) {
        if (!running_f) return;
        upd_f = 1;
        String answ;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("push"));
        _jsStr(answ, F("text"), text, true);
        _jsEnd(answ);
        _send(answ, true);
    }

    // отправить всплывающее уведомление
    void sendNotice(const String& text, uint32_t color = GH_GREEN) {
        if (!running_f || !focused()) return;
        upd_f = 1;
        String answ;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("notice"));
        _jsStr(answ, F("text"), text);
        _jsStr(answ, F("color"), color, true);
        _jsEnd(answ);
        _send(answ);
    }

    // показать окно с ошибкой
    void sendAlert(const String& text) {
        if (!running_f || !focused()) return;
        upd_f = 1;
        String answ;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("alert"));
        _jsStr(answ, F("text"), text, true);
        _jsEnd(answ);
        _send(answ);
    }

    // ========================= UPDATE ==========================

    // отправить update вручную с указанием значения
    void sendUpdate(const String& name, const String& value) {
        _sendUpdate(name.c_str(), value.c_str());
    }

    // отправить update по имени компонента (значение будет прочитано в build). Нельзя вызывать из build. Имена можно передать списком через запятую
    void sendUpdate(const String& name) {
        if (!running_f || !build_cb || bptr || !focused()) return;
        GHbuild build(GH_BUILD_READ);
        bptr = &build;

        String answ;
        sptr = &answ;
        _updateBegin(answ);

        char* str = (char*)name.c_str();
        char* p = str;
        GH_splitter(NULL);
        while ((p = GH_splitter(str)) != NULL) {
            build.type = GH_BUILD_READ;
            build.action.name = p;
            build.action.count = 0;
            answ += '\"';
            answ += p;
            answ += F("\":\"");
            answ.reserve(answ.length() + 64);
            build_cb();
            answ += F("\",");
        }
        bptr = nullptr;
        sptr = nullptr;
        answ[answ.length() - 1] = '}';
        _jsEnd(answ);
        _send(answ);
    }

    // автоматически рассылать обновления клиентам при действиях на странице (умолч. true)
    void sendUpdateAuto(bool f) {
        autoUpd_f = f;
    }

    void _updateBegin(String& answ) {
        upd_f = 1;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("update"));
        answ += F("\"updates\":{");
    }
    void _sendUpdate(const char* name, const char* value) {
        if (!running_f || !focused()) return;
        String answ;
        _updateBegin(answ);
        answ += '\"';
        answ += name;
        answ += F("\":\"");
        GH_addEsc(&answ, value);//answ += value;
        answ += F("\"}}\n");
        _send(answ);
    }

    // ======================= SEND CANVAS ========================
    // отправить холст
    void sendCanvas(const String& name, GHcanvas& cv) {
        if (!running_f) return;
        String answ;
        _updateBegin(answ);
        answ += '\"';
        answ += name;
        answ += F("\":[");
        answ += cv.buf;
        answ += F("]}}\n");
        _send(answ);
        cv.clearBuffer();
    }

    // начать отправку холста
    void sendCanvasBegin(const String& name, GHcanvas& cv) {
        if (!running_f) return;
        cv.buf = "";
        _updateBegin(cv.buf);
        cv.buf += '\"';
        cv.buf += name;
        cv.buf += F("\":[");
    }

    // закончить отправку холста
    void sendCanvasEnd(GHcanvas& cv) {
        cv.buf += F("]}}\n");
        _send(cv.buf);
        cv.clearBuffer();
    }

    // ======================== SEND GET =========================

    // автоматически отправлять новое состояние на get-топик при изменении через set (умолч. false)
    void sendGetAuto(bool v) {
#ifdef GH_ESP_BUILD
        autoGet_f = v;
#endif
    }

    // отправить имя-значение на get-топик (MQTT)
    void sendGet(GH_UNUSED const String& name, GH_UNUSED const String& value) {
        if (!running_f) return;
#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
        String topic(prefix);
        topic += F("/hub/");
        topic += id;
        topic += F("/get/");
        topic += name;
        sendMQTT(topic, value);
#endif
#endif
    }

    // отправить значение по имени компонента на get-топик (MQTT) (значение будет прочитано в build). Имена можно передать списком через запятую
    void sendGet(GH_UNUSED const String& name) {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
        if (!running_f || !build_cb || bptr) return;
        GHbuild build(GH_BUILD_READ);
        bptr = &build;

        String value;
        sptr = &value;

        char* str = (char*)name.c_str();
        char* p = str;
        GH_splitter(NULL);
        while ((p = GH_splitter(str)) != NULL) {
            build.type = GH_BUILD_READ;
            build.action.name = p;
            build.action.count = 0;
            build_cb();
            if (build.type == GH_BUILD_NONE) sendGet(p, value);
        }
        bptr = nullptr;
        sptr = nullptr;
#endif
#endif
    }

    // ========================== ON/OFF ==========================

    // отправить MQTT LWT команду на включение
    void turnOn() {
        _power(F("online"));
    }

    // отправить MQTT LWT команду на выключение
    void turnOff() {
        _power(F("offline"));
    }

    // ========================== PARSER ==========================
    void parse(char* url, GHconn_t from) {
        parse(url, from, GH_ESP);
    }
    void parse(char* url, char* value, GHconn_t from) {
        parse(url, value, from, GH_ESP);
    }

    // парсить строку вида PREFIX/ID/HUB_ID/CMD/NAME=VALUE
    void parse(char* url, GHconn_t from, GHsource_t source) {
        if (!running_f) return;
        char* eq = strchr(url, '=');
        char val[1] = "";
        if (eq) *eq = 0;
        parse(url, eq ? (eq + 1) : val, from, source);
        if (eq) *eq = '=';
    }

    // парсить строку вида PREFIX/ID/HUB_ID/CMD/NAME с отдельным value
    void parse(char* url, char* value, GHconn_t from, GHsource_t source) {
        if (!running_f) return;
        if (!modules.read(GH_MOD_SERIAL) && from == GH_SERIAL) return;
        if (!modules.read(GH_MOD_BT) && from == GH_BT) return;
        if (!modules.read(GH_MOD_WS) && from == GH_WS) return;
        if (!modules.read(GH_MOD_MQTT) && from == GH_MQTT) return;

#if defined(GH_ESP_BUILD) && !defined(GH_NO_FS) && !defined(GH_NO_OTA) && !defined(GH_NO_OTA_URL)
        if (ota_url_f) return;
#endif

        if (!strcmp(url, prefix)) {  // == prefix
            GHclient client(from, value, source);
            client_ptr = &client;
            answerDiscover();
            return sendEvent(GH_DISCOVER_ALL, from);
        }

        GHparser<5> p(url);
        if (strcmp(p.str[0], prefix)) return;  // wrong prefix
        if (strcmp(p.str[1], id)) return;      // wrong id

        if (p.size == 2) {
            GHclient client(from, value, source);
            client_ptr = &client;
            answerDiscover();
            return sendEvent(GH_DISCOVER, from);
        }

        if (p.size == 3) return sendEvent(GH_UNKNOWN, from);
        // p.size >= 4

        const char* cmd = p.str[3];
        int cmdn = GH_getCmd(cmd);
        if (cmdn < 0) return sendEvent(GH_UNKNOWN, from);

        const char* client_id = p.str[2];
        const char* name = p.str[4];
        GHclient client(from, client_id, source);
        client_ptr = &client;

        if (req_cb) {
            if (!req_cb(GHbuild(GH_BUILD_NONE, name, value, client, (GHevent_t)cmdn))) {
                answerErr(F("Forbidden"));
                return;
            }
        }

        if (p.size == 4) {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
            // MQTT HOOK
            if (from == GH_MQTT && build_cb) {
                if (!strcmp_P(cmd, PSTR("read"))) {
                    if (modules.read(GH_MOD_READ)) sendGet(name);
                    client_ptr = nullptr;
                    return sendEvent(GH_READ_HOOK, from);
                } else if (!strcmp_P(cmd, PSTR("set"))) {
                    if (modules.read(GH_MOD_SET)) {
                        GHbuild build(GH_BUILD_ACTION, name, value, client);
                        bptr = &build;
                        build_cb();
                        bptr = nullptr;
                        client_ptr = nullptr;
                        if (autoGet_f) sendGet(name, value);
                        if (autoUpd_f) _sendUpdate(name, value);
                    }
                    return sendEvent(GH_SET_HOOK, from);
                }
            }
#endif
#endif
            setFocus(from);

            switch (cmdn) {
                case 0:  // focus
                    answerUI();
                    return sendEvent(GH_FOCUS, from);

                case 1:  // ping
                    answerType();
                    return sendEvent(GH_PING, from);

                case 2:  // unfocus
                    clearFocus(from);
                    return sendEvent(GH_UNFOCUS, from);

                case 3:  // info
                    if (modules.read(GH_MOD_INFO)) answerInfo();
                    return sendEvent(GH_INFO, from);

#ifdef GH_ESP_BUILD
                case 4:  // fsbr
#ifndef GH_NO_FS
                    if (modules.read(GH_MOD_FSBR)) {
                        if (fs_mounted) answerFsbr();
                        else answerType(F("fs_error"));
                    }
#else
                    answerDsbl();
#endif
                    return sendEvent(GH_FSBR, from);

                case 5:  // format
#ifndef GH_NO_FS
                    if (modules.read(GH_MOD_FORMAT)) {
                        GH_FS.format();
                        GH_FS.end();
                        fs_mounted = GH_FS.begin();
                        answerFsbr();
                    }
#else
                    answerDsbl();
#endif
                    return sendEvent(GH_FORMAT, from);

                case 6:  // reboot
                    if (modules.read(GH_MOD_REBOOT)) {
                        reboot_f = GH_REB_BUTTON;
                        answerType();
                    }
                    return sendEvent(GH_REBOOT, from);
#endif
            }
            return;
        }

        // p.size == 5
        setFocus(from);

        switch (cmdn) {
            case 7:  // data
                answ_f = 0;
                if (data_cb) data_cb(name, value);
                if (!answ_f) answerType();
                return sendEvent(GH_DATA, from);

            case 8:  // set
                if (!build_cb || !modules.read(GH_MOD_SET)) {
                    answerType();
                } else {
                    GHbuild build(GH_BUILD_ACTION, name, value, client);
                    bptr = &build;
                    upd_f = refresh_f = 0;
                    build_cb();
                    bptr = nullptr;
#ifdef GH_ESP_BUILD
                    if (autoGet_f) sendGet(name, value);
#endif
                    if (autoUpd_f) _sendUpdate(name, value);
                    if (refresh_f) answerUI();
                    else if (!upd_f) answerType();
                }
                return sendEvent(GH_SET, from);

            case 9:  // cli
                answerType();
                if (cli_cb) {
                    String str(value);
                    cli_cb(str);
                }
                return sendEvent(GH_CLI, from);

#ifdef GH_ESP_BUILD
            case 10:  // delete
#ifndef GH_NO_FS
                if (modules.read(GH_MOD_DELETE)) {
                    GH_FS.remove(name);
                    _fsrmdir(name);
                    answerFsbr();
                }
#else
                answerDsbl();
#endif
                return sendEvent(GH_DELETE, from);

            case 11:  // rename
#ifndef GH_NO_FS
                if (modules.read(GH_MOD_RENAME) && GH_FS.rename(name, value)) answerFsbr();
#else
                answerDsbl();
#endif
                return sendEvent(GH_RENAME, from);

            case 12:  // fetch
#ifndef GH_NO_FS
                if (!file_d && !file_b && !file_u && !ota_f && modules.read(GH_MOD_DOWNLOAD)) {
                    fetch_path = name;
                    if (fetch_cb) fetch_cb(fetch_path, true);
                    if (!file_d && !file_b) file_d = GH_FS.open(name, "r");
                    if (file_d || file_b) {
                        fs_client = client;
                        fs_tmr = millis();
                        uint32_t size = file_b ? file_b_size : file_d.size();
                        file_b_idx = 0;
                        dwn_chunk_count = 0;
                        dwn_chunk_amount = (size + GH_DOWN_CHUNK_SIZE - 1) / GH_DOWN_CHUNK_SIZE;  // round up
                        answerType(F("fetch_start"));
                        return sendEvent(GH_DOWNLOAD, from);
                    }
                }
#endif
                answerType(F("fetch_err"));
                return sendEvent(GH_DOWNLOAD_ERROR, from);

            case 13:  // fetch_chunk
#ifndef GH_NO_FS
                fs_tmr = millis();
                if ((!file_d && !file_b) || fs_client != client || !modules.read(GH_MOD_DOWNLOAD)) {
                    answerType(F("fetch_err"));
                    return sendEvent(GH_DOWNLOAD_ERROR, from);
                } else {
                    answerChunk();
                    dwn_chunk_count++;
                    if (dwn_chunk_count >= dwn_chunk_amount) {
                        if (fetch_cb) fetch_cb(fetch_path, false);
                        if (file_d) file_d.close();
                        file_b = nullptr;
                        fetch_path = "";
                        return sendEvent(GH_DOWNLOAD_FINISH, from);
                    }
                    return sendEvent(GH_DOWNLOAD_CHUNK, from);
                }
#endif
                break;

            case 14:  // fetch_stop
#ifndef GH_NO_FS
                if (fetch_cb) fetch_cb(fetch_path, false);
                if (file_d) file_d.close();
                file_b = nullptr;
                fetch_path = "";
                sendEvent(GH_DOWNLOAD_ABORTED, fs_client.from);
#endif
                break;

            case 15:  // upload
#ifndef GH_NO_FS
                if (!file_d && !file_b && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_UPLOAD)) {
                    _fsmakedir(name);
                    file_u = GH_FS.open(name, "w");
                    if (file_u) {
                        fs_buffer = (char*)malloc(GH_UPL_CHUNK_SIZE + 10);
                        if (fs_buffer) {
                            fs_client = client;
                            fs_tmr = millis();
                            answerType(F("upload_start"));
                            sendEvent(GH_UPLOAD, from);
                            return;
                        }
                    }
                }
#endif
                answerType(F("upload_err"));
                return sendEvent(GH_UPLOAD_ERROR, from);

            case 16:  // upload_chunk
#ifndef GH_NO_FS
                if (file_u && fs_client == client && fs_buffer) {
                    if (!strcmp_P(name, PSTR("next"))) {
                        fs_state = GH_UPLOAD_CHUNK;
                        strcpy(fs_buffer, value);
                        return;
                    } else if (!strcmp_P(name, PSTR("last"))) {
                        fs_state = GH_UPLOAD_FINISH;
                        strcpy(fs_buffer, value);
                        return;
                    }
                }
#endif
                answerType(F("upload_err"));
                return sendEvent(GH_UPLOAD_ERROR, from);

            case 17:  // ota
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA)
                if (!file_d && !file_b && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_OTA)) {
                    int ota_type = 0;
                    if (!strcmp_P(name, PSTR("flash"))) ota_type = 1;
                    else if (!strcmp_P(name, PSTR("fs"))) ota_type = 2;

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
                        if (Update.begin(ota_size, ota_type)) {
                            fs_buffer = (char*)malloc(GH_UPL_CHUNK_SIZE + 10);
                            if (fs_buffer) {
                                fs_client = client;
                                ota_f = true;
                                fs_tmr = millis();
                                answerType(F("ota_start"));
                                return sendEvent(GH_OTA, from);
                            }
                        }
                    }
                }
#endif
                answerType(F("ota_err"));
                return sendEvent(GH_OTA_ERROR, from);

            case 18:  // ota_chunk
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA)
                if (ota_f && fs_client == client && fs_buffer) {
                    if (!strcmp_P(name, PSTR("next"))) {
                        fs_state = GH_OTA_CHUNK;
                        strcpy(fs_buffer, value);
                        return;
                    } else if (!strcmp_P(name, PSTR("last"))) {
                        fs_state = GH_OTA_FINISH;
                        strcpy(fs_buffer, value);
                        return;
                    }
                }
#endif
                answerType(F("ota_err"));
                return sendEvent(GH_OTA_ERROR, from);

            case 19:  // ota_url
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA) && !defined(GH_NO_OTA_URL)
                if (!file_d && !file_b && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_OTA_URL)) {
                    if (!strcmp_P(name, PSTR("flash"))) ota_url_fs = 0;
                    else if (!strcmp_P(name, PSTR("fs"))) ota_url_fs = 1;
                    ota_url = value;
                    answerType();
                    fs_state = GH_OTA_URL;
                    return sendEvent(GH_OTA_URL, from);
                }
#endif
                answerErr(F("File busy"));
                return sendEvent(GH_OTA_URL, from);
#endif
        }
    }

    // ========================== TICK ==========================

    // тикер, вызывать в loop
    bool tick() {
        if (!running_f) return 0;

        if ((uint16_t)((uint16_t)millis() - focus_tmr) >= 1000) {
            focus_tmr = millis();
            for (uint8_t i = 0; i < GH_CONN_AMOUNT; i++) {
                if (focus_arr[i]) focus_arr[i]--;
            }
        }

#ifndef GH_NO_STREAM
        tickStream();
#endif
#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
        tickWS();
        tickHTTP();
#endif
#ifndef GH_NO_MQTT
        tickMQTT();
#endif

#ifndef GH_NO_FS
        if ((file_d || file_b || file_u || ota_f) && (uint16_t)millis() - fs_tmr >= (GH_CONN_TOUT * 1000)) {
            if (file_d || file_b) fs_state = GH_DOWNLOAD_ABORTED;
            if (file_u) fs_state = GH_UPLOAD_ABORTED;
            if (ota_f) fs_state = GH_OTA_ABORTED;
            if (fs_buffer) {
                delete fs_buffer;
                fs_buffer = nullptr;
            }
        }

        if (fs_state != GH_IDLE) {
            switch (fs_state) {
#ifndef GH_NO_OTA_URL
                case GH_OTA_URL: {
                    bool ok = 0;
                    ota_url_f = 1;
#ifdef ESP8266
                    ESPhttpUpdate.rebootOnUpdate(false);
                    BearSSL::WiFiClientSecure client;
                    if (ota_url.startsWith(F("https"))) client.setInsecure();
                    if (ota_url_fs) ESPhttpUpdate.updateFS(client, ota_url);
                    else ok = ESPhttpUpdate.update(client, ota_url);
#else
                    httpUpdate.rebootOnUpdate(false);
                    WiFiClientSecure client;
                    if (ota_url.startsWith(F("https"))) client.setInsecure();
                    if (ota_url_fs) ok = httpUpdate.updateSpiffs(client, ota_url);
                    else ok = httpUpdate.update(client, ota_url);
#endif
                    client_ptr = &fs_client;
                    if (ok) {
                        reboot_f = GH_REB_OTA_URL;
                        answerType(F("ota_url_ok"));
                    } else {
                        ota_url = "";
                        ota_url_f = 0;
                        answerType(F("ota_url_err"));
                    }
                } break;
#endif

                case GH_DOWNLOAD_ABORTED:
                    if (fetch_cb) fetch_cb(fetch_path, false);
                    if (file_d) file_d.close();
                    file_b = nullptr;
                    fetch_path = "";
                    sendEvent(GH_DOWNLOAD_ABORTED, fs_client.from);
                    break;

                case GH_UPLOAD_CHUNK:
                    GH_B64toFile(file_u, fs_buffer);
                    client_ptr = &fs_client;
                    answerType(F("upload_next_chunk"));
                    fs_tmr = millis();
                    sendEvent(GH_UPLOAD_CHUNK, fs_client.from);
                    break;

                case GH_UPLOAD_FINISH:
                    GH_B64toFile(file_u, fs_buffer);
                    delete fs_buffer;
                    fs_buffer = nullptr;
                    if (file_u) file_u.close();
                    client_ptr = &fs_client;
                    answerType(F("upload_end"));
                    sendEvent(GH_UPLOAD_FINISH, fs_client.from);
                    break;

                case GH_UPLOAD_ABORTED:
                    if (file_u) file_u.close();
                    sendEvent(GH_UPLOAD_ABORTED, fs_client.from);
                    break;
#ifndef GH_NO_OTA
                case GH_OTA_CHUNK:
                    GH_B64toUpdate(fs_buffer);
                    client_ptr = &fs_client;
                    answerType(F("ota_next_chunk"));
                    fs_tmr = millis();
                    sendEvent(GH_OTA_CHUNK, fs_client.from);
                    break;

                case GH_OTA_FINISH:
                    GH_B64toUpdate(fs_buffer);
                    delete fs_buffer;
                    fs_buffer = nullptr;
                    ota_f = false;
                    reboot_f = GH_REB_OTA;
                    client_ptr = &fs_client;
                    if (Update.end(true)) answerType(F("ota_end"));
                    else answerType(F("ota_err"));
                    sendEvent(GH_OTA_FINISH, fs_client.from);
                    break;

                case GH_OTA_ABORTED:
                    Update.end();
                    ota_f = false;
                    sendEvent(GH_OTA_ABORTED, fs_client.from);
                    break;
#endif
                default:
                    break;
            }
            fs_state = GH_IDLE;
        }
#endif
        if (reboot_f) {
            if (reboot_cb) reboot_cb(reboot_f);
            delay(2000);
            ESP.restart();
        }
#endif
        return 1;
    }

    // =========================================================================================
    // ======================================= PRIVATE =========================================
    // =========================================================================================
   private:
    void _rebootOTA() {
#ifdef GH_ESP_BUILD
        reboot_f = GH_REB_OTA;
#endif
    }
    const char* getPrefix() {
        return prefix;
    }
    const char* getID() {
        return id;
    }
    void _afterComponent() {
        switch (buf_mode) {
            case GH_NORMAL:
                break;

            case GH_COUNT:
                buf_count += sptr->length();
                *sptr = "";
                break;

            case GH_CHUNKED:
                if (sptr->length() >= buf_size) {
                    _answer(*sptr, false);
                    *sptr = "";
                }
                break;
        }
    }
    void _power(FSTR mode) {
        if (!running_f) return;

#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
        String topic(prefix);
        topic += F("/hub/");
        topic += id;
        topic += F("/status");
        sendMQTT(topic, mode);
#endif
#endif
    }
    void _fsmakedir(const char* path) {
#ifdef ESP32
        if (!GH_FS.exists(path)) {
            if (strchr(path, '/')) {
                char* pathStr = strdup(path);
                if (pathStr) {
                    char* ptr = strchr(pathStr, '/');
                    while (ptr) {
                        *ptr = 0;
                        GH_FS.mkdir(pathStr);
                        *ptr = '/';
                        ptr = strchr(ptr + 1, '/');
                    }
                }
                free(pathStr);
            }
        }
#endif
    }
    void _fsrmdir(const char* path) {
#ifdef ESP32
        char* pathStr = strdup(path);
        if (pathStr) {
            char* ptr = strrchr(pathStr, '/');
            while (ptr) {
                *ptr = 0;
                GH_FS.rmdir(pathStr);
                ptr = strrchr(pathStr, '/');
            }
            free(pathStr);
        }
#endif
    }

    // ======================= INFO ========================
    void answerInfo() {
        String answ;
        answ.reserve(250);
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("info"));

        answ += F("\"info\":{\"version\":{");
        _jsStr(answ, F("Library"), GH_LIB_VERSION);
        if (version) _jsStr(answ, F("Firmware"), version);

        checkEndInfo(answ, GH_INFO_VERSION);
        answ += (F(",\"net\":{"));
#ifdef GH_ESP_BUILD
        _jsStr(answ, F("Mode"), WiFi.getMode() == WIFI_AP ? F("AP") : (WiFi.getMode() == WIFI_STA ? F("STA") : F("AP_STA")));
        _jsStr(answ, F("MAC"), WiFi.macAddress());
        _jsStr(answ, F("SSID"), WiFi.SSID());
        _jsStr(answ, F("RSSI"), String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + '%');
        _jsStr(answ, F("IP"), WiFi.localIP().toString());
        _jsStr(answ, F("AP_IP"), WiFi.softAPIP().toString());
#endif
        checkEndInfo(answ, GH_INFO_NETWORK);
        answ += (F(",\"memory\":{"));

#ifdef GH_ESP_BUILD
        _jsVal(answ, F("RAM"), String("[") + ESP.getFreeHeap() + ",0]");

#ifndef GH_NO_FS
#ifdef ESP8266
        FSInfo fs_info;
        GH_FS.info(fs_info);
        _jsVal(answ, F("Flash"), String("[") + fs_info.usedBytes + ',' + fs_info.totalBytes + "]");
#else
        _jsVal(answ, F("Flash"), String("[") + GH_FS.usedBytes() + ',' + GH_FS.totalBytes() + "]");
#endif

        _jsVal(answ, F("Sketch"), String("[") + ESP.getSketchSize() + ',' + ESP.getFreeSketchSpace() + "]");
#endif
#endif

        checkEndInfo(answ, GH_INFO_MEMORY);
        answ += (F(",\"system\":{"));
        _jsVal(answ, F("Uptime"), millis() / 1000ul);
#ifdef GH_ESP_BUILD
#ifdef ESP8266
        _jsStr(answ, F("Platform"), F("ESP8266"));
#else
        _jsStr(answ, F("Platform"), F("ESP32"));
#endif
        _jsVal(answ, F("CPU_MHz"), ESP.getCpuFreqMHz());
        _jsStr(answ, F("Flash_chip"), String(ESP.getFlashChipSize() / 1000.0, 1) + " kB");
#endif

#ifdef __AVR_ATmega328P__
        _jsStr(answ, F("Platform"), F("ATmega328"));
#endif

        checkEndInfo(answ, GH_INFO_SYSTEM);
        answ += (F("}"));

        _jsEnd(answ);
        _answer(answ);
    }
    void checkEndInfo(String& answ, GHinfo_t info) {
        if (info_cb) {
            sptr = &answ;
            info_cb(info);
            sptr = nullptr;
        }
        if (answ[answ.length() - 1] == ',') answ[answ.length() - 1] = '}';
        else answ += '}';
    }

    // ======================= UI ========================
    void answerUI() {
        if (!build_cb) return answerType();
        GHbuild build;
        build.client = *client_ptr;
        bptr = &build;
        bool chunked = buf_size;

#ifdef GH_ESP_BUILD
        if (build.client.from == GH_WS || build.client.from == GH_MQTT) chunked = false;
#endif

        if (!chunked) {
            build.type = GH_BUILD_COUNT;
            build.action.count = 0;
            buf_mode = GH_COUNT;
            buf_count = 0;
            String count;
            sptr = &count;
            tab_width = 0;
            build_cb();
        }

        String answ;
        answ.reserve((chunked ? buf_size : buf_count) + 100);
        answ = F("\n{\"controls\":[");
        buf_mode = chunked ? GH_CHUNKED : GH_NORMAL;
        build.type = GH_BUILD_UI;
        build.action.count = 0;
        sptr = &answ;
        tab_width = 0;
        build_cb();
        sptr = nullptr;
        bptr = nullptr;

        if (answ[answ.length() - 1] == ',') answ[answ.length() - 1] = ']';  // ',' = ']'
        else answ += ']';
        answ += ',';
        _jsID(answ);
        _jsStr(answ, F("type"), F("ui"), true);
        _jsEnd(answ);
        _answer(answ);
    }

    // ======================= TYPE ========================
    void answerType(FSTR type = nullptr) {
        if (!type) type = F("OK");
        String answ;
        answ.reserve(50);
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), type, true);
        _jsEnd(answ);
        _answer(answ);
    }
    void answerErr(FSTR err) {
        String answ;
        answ.reserve(50);
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("ERR"));
        _jsStr(answ, F("text"), err, true);
        _jsEnd(answ);
        _answer(answ);
    }
    void answerDsbl() {
        answerErr(F("Module disabled"));
    }

    // ======================= FSBR ========================
    void answerFsbr() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
        uint16_t count = 0;
        String answ;
        answ.reserve(100);
        GH_showFiles(answ, "/", GH_FS_DEPTH, &count);
        answ.reserve(count + 50);
        answ = F("\n{\"fs\":{\"/\":0,");
        GH_showFiles(answ, "/", GH_FS_DEPTH);
        answ[answ.length() - 1] = '}';  // ',' = '}'
        answ += ',';

        _jsID(answ);
        _jsStr(answ, F("type"), F("fsbr"));

#ifdef ESP8266
        FSInfo fs_info;
        GH_FS.info(fs_info);
        _jsVal(answ, F("total"), fs_info.totalBytes);
        _jsVal(answ, F("used"), fs_info.usedBytes, true);
#else
        _jsVal(answ, F("total"), GH_FS.totalBytes());
        _jsVal(answ, F("used"), GH_FS.usedBytes(), true);
#endif
        _jsEnd(answ);
        _answer(answ);
#endif
#else
        answerDsbl();
#endif
    }

    // ======================= DISCOVER ========================
    void answerDiscover() {
        uint32_t hash = 0;
        if (PIN > 999) {
            char pin_s[11];
            ultoa(PIN, pin_s, 10);
            uint16_t len = strlen(pin_s);
            for (uint16_t i = 0; i < len; i++) {
                hash = (((uint32_t)hash << 5) - hash) + pin_s[i];
            }
        }

        String answ;
        answ.reserve(120);
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("discover"));
        _jsStr(answ, F("name"), name);
        _jsStr(answ, F("icon"), icon);
        _jsVal(answ, F("PIN"), hash);
        _jsStr(answ, F("version"), version);
        _jsVal(answ, F("max_upl"), GH_UPL_CHUNK_SIZE);
#ifdef ATOMIC_FS_UPDATE
        _jsStr(answ, F("ota_t"), F("gz"));
#else
        _jsStr(answ, F("ota_t"), F("bin"));
#endif

#ifdef GH_NO_FS
        modules.unset(GH_MOD_FSBR | GH_MOD_FORMAT | GH_MOD_DOWNLOAD | GH_MOD_UPLOAD | GH_MOD_OTA | GH_MOD_OTA_URL | GH_MOD_DELETE | GH_MOD_RENAME);
#endif
#ifdef GH_NO_OTA
        modules.unset(GH_MOD_OTA);
#endif
#ifdef GH_NO_OTA_URL
        modules.unset(GH_MOD_OTA_URL);
#endif
#ifndef GH_ESP_BUILD
        modules.unset(GH_MOD_REBOOT | GH_MOD_FSBR | GH_MOD_FORMAT | GH_MOD_DOWNLOAD | GH_MOD_UPLOAD | GH_MOD_OTA | GH_MOD_OTA_URL | GH_MOD_DELETE | GH_MOD_RENAME);
#endif
        _jsVal(answ, F("modules"), modules.mods, true);
        _jsEnd(answ);
        _answer(answ, true);
    }

    // ======================= CHUNK ========================
    void answerChunk() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
        String answ;
        answ.reserve(GH_DOWN_CHUNK_SIZE + 100);
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("fetch_next_chunk"));
        _jsVal(answ, F("chunk"), dwn_chunk_count);
        _jsVal(answ, F("amount"), dwn_chunk_amount);
        answ += F("\"data\":\"");
        if (file_b) GH_bytesToB64(file_b, file_b_idx, file_b_size, answ);
        else GH_fileToB64(file_d, answ);
        answ += '\"';
        _jsEnd(answ);
        _answer(answ);
#endif
#endif
    }

    // ======================= ANSWER ========================
    void _answer(String& answ, bool close = true) {
        if (!client_ptr) return;
        switch (client_ptr->source) {
            case GH_ESP:
#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
                if (client_ptr->from == GH_WS) answerWS(answ);
#endif
#ifndef GH_NO_MQTT
                if (client_ptr->from == GH_MQTT) answerMQTT(answ, client_ptr->id);
#endif
#endif
                break;
            case GH_MANUAL:
                if (manual_cb) manual_cb(answ, client_ptr->from, false);

                break;
            case GH_STREAM:
#ifndef GH_NO_STREAM
                sendStream(answ);
#endif
                break;
        }
        if (close) client_ptr = nullptr;
    }

    // ======================= SEND ========================
    void _send(String& answ, bool broadcast = false) {
        if (manual_cb) {
            for (int i = 0; i < GH_CONN_AMOUNT; i++) {
                manual_cb(answ, (GHconn_t)i, broadcast);
            }
        }

#ifndef GH_NO_STREAM
        if (stateStream() && focus_arr[connStream()]) sendStream(answ);
#endif

#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
        if (focus_arr[GH_WS]) sendWS(answ);
#endif
#ifndef GH_NO_MQTT
        if (focus_arr[GH_MQTT] || broadcast) sendMQTT(answ);
#endif
#endif
    }
    void _datasend(String& answ, const String& data) {
        answ_f = 1;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("data"));
        _jsStr(answ, F("data"), data, true);
        _jsEnd(answ);
    }

    // ========================== MISC ==========================
    void setFocus(GHconn_t from) {
        focus_arr[from] = GH_CONN_TOUT;
    }
    void clearFocus(GHconn_t from) {
        focus_arr[from] = 0;
        client_ptr = nullptr;
    }

    // ========================== ADDER ==========================
    template <typename T>
    void _jsVal(String& s, FSTR key, T value, bool last = false) {
        s += '\"';
        s += key;
        s += F("\":");
        s += value;
        if (!last) s += ',';
    }
    template <typename T>
    void _jsStr(String& s, FSTR key, T value, bool last = false) {
        s += '\"';
        s += key;
        s += F("\":\"");
        s += value;
        s += '\"';
        if (!last) s += ',';
    }
    void _jsStr(String& s, FSTR key, const String& value, bool last = false) {
        s += '\"';
        s += key;
        s += F("\":\"");
        GH_addEsc(&s, value.c_str());
        s += '\"';
        if (!last) s += ',';
    }
    void _jsID(String& s, bool last = false) {
        s += F("\"id\":\"");
        s += id;
        s += '\"';
        if (!last) s += ',';
    }
    void _jsBegin(String& s) {
        s += F("\n{");
    }
    void _jsEnd(String& s) {
        s += F("}\n");
    }

    // ========================== VARS ==========================
    const char* prefix = nullptr;
    const char* name = nullptr;
    const char* icon = nullptr;
    const char* version = nullptr;
    uint32_t PIN = 0;
    char id[9];

    void (*fetch_cb)(String& path, bool start) = nullptr;
    void (*data_cb)(const char* name, const char* value) = nullptr;
    void (*build_cb)() = nullptr;
    bool (*req_cb)(GHbuild build) = nullptr;
    void (*info_cb)(GHinfo_t info) = nullptr;
    void (*cli_cb)(String& str) = nullptr;
    void (*manual_cb)(String& s, GHconn_t from, bool broadcast) = nullptr;
    void (*event_cb)(GHevent_t state, GHconn_t from) = nullptr;
    GHclient* client_ptr = nullptr;

    bool running_f = 0;
    bool refresh_f = 0;
    bool upd_f = 0;
    bool answ_f = 0;

    enum GHbuildmode_t {
        GH_NORMAL,
        GH_COUNT,
        GH_CHUNKED,
    };
    GHbuildmode_t buf_mode = GH_NORMAL;
    uint16_t buf_size = 0;
    uint16_t buf_count = 0;

    uint16_t focus_tmr = 0;
    int8_t focus_arr[GH_CONN_AMOUNT] = {};
    bool autoUpd_f = true;

#ifdef GH_ESP_BUILD
    void (*reboot_cb)(GHreason_t r) = nullptr;
    bool autoGet_f = 0;
    GHreason_t reboot_f = GH_REB_NONE;

#ifndef GH_NO_FS
#ifndef GH_NO_OTA_URL
    String ota_url;
    bool ota_url_f = 0;
    bool ota_url_fs = 0;
#endif
    bool fs_mounted = 0;
    GHclient fs_client;
    GHevent_t fs_state = GH_IDLE;
    char* fs_buffer = nullptr;
    String fetch_path;
    uint8_t* file_b = nullptr;
    uint32_t file_b_size, file_b_idx;
    File file_d, file_u;
    bool ota_f = false;
    uint16_t dwn_chunk_count = 0;
    uint16_t dwn_chunk_amount = 0;
    uint16_t fs_tmr = 0;
#endif
#endif
};
#endif