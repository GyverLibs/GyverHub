#ifndef _GyverHUB_h
#define _GyverHUB_h

#include <Arduino.h>

#include "builder.h"
#include "canvas.h"
#include "config.hpp"
#include "macro.hpp"
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
class GyverHub : public HubBuilder, public HubHTTP, public HubMQTT, public HubWS {
#else
class GyverHub : public HubBuilder {
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
    void onManual(void (*handler)(String& s, GHconn_t conn, bool broadcast)) {
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
            *sptr += '\'';
            *sptr += label;
            *sptr += F("':'");
            *sptr += text;
            *sptr += F("',");
        }
    }

    // ========================= CLI =========================

    // подключить обработчик входящих сообщений с веб-консоли
    void onCLI(void (*handler)(String& s)) {
        cli_cb = *handler;
    }

    // отправить текст в веб-консоль. Опционально цвет
    void print(const String& s, uint32_t color = GH_DEFAULT) {
        if (!focused()) return;
        String answ;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("print"));
        _jsStr(answ, F("text"), s);
        _jsVal(answ, F("color"), color, true);
        _jsEnd(answ);
        send(answ);
    }

    // ========================== STATUS ==========================

    // подключить обработчик изменения статуса
    void onEvent(void (*handler)(GHevent_t state, GHconn_t conn)) {
        event_cb = *handler;
    }

    // отправить event для отладки
    void sendEvent(GHevent_t state, GHconn_t conn) {
        if (event_cb) event_cb(state, conn);
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
    bool focused(GHconn_t conn) {
        return focus_arr[conn];
    }

    // обновить веб-интерфейс. Вызывать внутри обработчика build
    void refresh() {
        refresh_f = true;
    }

    // true - если билдер вызван для set или read операций
    bool buildRead() {
        return (bptr && (bptr->type == GH_BUILD_ACTION || bptr->type == GH_BUILD_READ));
    }

    // получить текущее действие для ручной обработки значений
    const GHaction& action() const {
        return bptr->action;
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
        send(answ, true);
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
        send(answ);
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
        send(answ);
    }

    // ========================= UPDATE ==========================

    // отправить update вручную с указанием значения
    void sendUpdate(const String& name, const String& value) {
        if (!running_f || !focused()) return;
        String answ;
        _updateBegin(answ);
        answ += '\'';
        answ += name;
        answ += F("':'");
        answ += value;
        answ += F("'}}\n");
        send(answ);
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
            build.count = 0;
            answ += '\'';
            answ += p;
            answ += F("':'");
            answ.reserve(answ.length() + 64);
            build_cb();
            answ += F("',");
        }
        bptr = nullptr;
        sptr = nullptr;
        answ[answ.length() - 1] = '}';
        _jsEnd(answ);
        send(answ);
    }
    void _updateBegin(String& answ) {
        upd_f = 1;
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("update"));
        answ += F("'updates':{");
    }

    // ======================= SEND CANVAS ========================
    // отправить холст
    void sendCanvas(const String& name, GHcanvas& cv) {
        if (!running_f) return;
        String answ;
        _updateBegin(answ);
        answ += '\'';
        answ += name;
        answ += F("':[");
        answ += cv.buf;
        answ += F("]}}\n");
        send(answ);
        cv.clearBuffer();
    }

    // начать отправку холста
    void sendCanvasBegin(const String& name, GHcanvas& cv) {
        if (!running_f) return;
        cv.buf = "";
        _updateBegin(cv.buf);
        cv.buf += '\'';
        cv.buf += name;
        cv.buf += F("':[");
    }

    // закончить отправку холста
    void sendCanvasEnd(GHcanvas& cv) {
        cv.buf += F("]}}\n");
        send(cv.buf);
        cv.clearBuffer();
    }

    // ======================== SEND GET =========================

    // автоматически отправлять новое состояние на get-топик при изменении через set (умолч. false)
    void sendGetAuto(bool v) {
#ifdef GH_ESP_BUILD
        auto_f = v;
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
            build.count = 0;
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

    // парсить строку вида PREFIX/ID/HUB_ID/CMD/NAME=VALUE
    void parse(char* url, GHconn_t conn, bool manual = true) {
        if (!running_f) return;
        char* eq = strchr(url, '=');
        char val[1] = "";
        if (eq) *eq = 0;
        parse(url, eq ? (eq + 1) : val, conn, manual);
        if (eq) *eq = '=';
    }

    // парсить строку вида PREFIX/ID/HUB_ID/CMD/NAME с отдельным value
    void parse(char* url, char* value, GHconn_t conn, bool manual = true) {
        if (!modules.read(GH_MOD_SERIAL) && conn == GH_SERIAL) return;
        if (!modules.read(GH_MOD_BT) && conn == GH_BT) return;
        if (!modules.read(GH_MOD_WS) && conn == GH_WS) return;
        if (!modules.read(GH_MOD_MQTT) && conn == GH_MQTT) return;

#if defined(GH_ESP_BUILD) && !defined(GH_NO_FS) && !defined(GH_NO_OTA) && !defined(GH_NO_OTA_URL)
        if (ota_url_f) return;
#endif
        if (!running_f) return;

        if (!strcmp(url, prefix)) {  // == prefix
            GHhub hub(conn, value, manual);
            hub_ptr = &hub;
            answerDiscover();
            return sendEvent(GH_DISCOVER_ALL, conn);
        }

        GHparser<5> p(url);
        if (strcmp(p.str[0], prefix)) return;  // wrong prefix
        if (strcmp(p.str[1], id)) return;      // wrong id

        if (p.size == 2) {
            GHhub hub(conn, value, manual);
            hub_ptr = &hub;
            answerDiscover();
            return sendEvent(GH_DISCOVER, conn);
        }

        if (p.size == 3) return sendEvent(GH_UNKNOWN, conn);

        GHhub hub(conn, p.str[2], manual);
        hub_ptr = &hub;

        if (p.size == 4) {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
            // MQTT HOOK
            if (conn == GH_MQTT && build_cb) {
                if (!strcmp_P(p.str[2], PSTR("read"))) {
                    if (modules.read(GH_MOD_READ)) sendGet(p.str[3]);
                    hub_ptr = nullptr;
                    return sendEvent(GH_READ_HOOK, conn);
                } else if (!strcmp_P(p.str[2], PSTR("set"))) {
                    if (modules.read(GH_MOD_SET)) {
                        GHbuild build(GH_BUILD_ACTION, 1, p.str[3], value, hub);
                        bptr = &build;
                        build_cb();
                        bptr = nullptr;
                        hub_ptr = nullptr;
                        if (auto_f) sendGet(p.str[3], value);
                    }
                    return sendEvent(GH_SET_HOOK, conn);
                }
            }
#endif
#endif
            setFocus(conn);

            switch (GH_getCmd(p.str[3])) {
                case 0:  // focus
                    answerUI();
                    return sendEvent(GH_FOCUS, conn);

                case 1:  // ping
                    answerType();
                    return sendEvent(GH_PING, conn);

                case 2:  // unfocus
                    clearFocus(conn);
                    return sendEvent(GH_UNFOCUS, conn);

                case 3:  // info
                    if (modules.read(GH_MOD_INFO)) answerInfo();
                    return sendEvent(GH_INFO, conn);

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
                    return sendEvent(GH_FSBR, conn);

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
                    return sendEvent(GH_FORMAT, conn);

                case 6:  // reboot
                    if (modules.read(GH_MOD_REBOOT)) {
                        reboot_f = GH_REB_BUTTON;
                        answerType();
                    }
                    return sendEvent(GH_REBOOT, conn);

                case 7:  // fetch_chunk
#ifndef GH_NO_FS
                    fs_tmr = millis();
                    if (!file_d || fs_hub != hub || !modules.read(GH_MOD_DOWNLOAD)) {
                        answerType(F("fetch_err"));
                        return sendEvent(GH_DOWNLOAD_ERROR, conn);
                    } else {
                        answerChunk();
                        dwn_chunk_count++;
                        if (dwn_chunk_count >= dwn_chunk_amount) {
                            file_d.close();
                            return sendEvent(GH_DOWNLOAD_FINISH, conn);
                        }
                        return sendEvent(GH_DOWNLOAD_CHUNK, conn);
                    }
#endif
                    break;
#endif
                default:  // unknown
                    clearFocus(conn);
                    return sendEvent(GH_UNKNOWN, conn);
            }
            return;
        }

        // p.size == 5
        setFocus(conn);
        char* name = p.str[4];
        switch (GH_getCmdN(p.str[3])) {
            // set
            case 0:
                if (!build_cb || !modules.read(GH_MOD_SET)) {
                    answerType();
                } else {
                    GHbuild build(GH_BUILD_ACTION, 1, name, value, hub);
                    bptr = &build;
                    upd_f = refresh_f = 0;
                    build_cb();
                    bptr = nullptr;
#ifdef GH_ESP_BUILD
                    if (auto_f) sendGet(name, value);
#endif
                    if (refresh_f) answerUI();
                    else if (!upd_f) answerType();
                }
                return sendEvent(GH_SET, conn);

            // cli
            case 1:
                answerType();
                if (cli_cb) {
                    String str(value);
                    cli_cb(str);
                }
                return sendEvent(GH_CLI, conn);

#ifdef GH_ESP_BUILD
            // delete
            case 2:
#ifndef GH_NO_FS
                if (modules.read(GH_MOD_DELETE)) {
                    GH_FS.remove(name);
                    _fsrmdir(name);
                    answerFsbr();
                }
#else
                answerDsbl();
#endif
                return sendEvent(GH_DELETE, conn);

            // rename
            case 3:
#ifndef GH_NO_FS
                if (modules.read(GH_MOD_RENAME) && GH_FS.rename(name, value)) answerFsbr();
#else
                answerDsbl();
#endif
                return sendEvent(GH_RENAME, conn);

            // fetch
            case 4:
#ifndef GH_NO_FS
                if (!file_d && !file_u && !ota_f && modules.read(GH_MOD_DOWNLOAD)) {
                    file_d = GH_FS.open(name, "r");
                    if (file_d) {
                        fs_hub = hub;
                        fs_tmr = millis();
                        dwn_chunk_count = 0;
                        dwn_chunk_amount = (file_d.size() + GH_DOWN_CHUNK_SIZE - 1) / GH_DOWN_CHUNK_SIZE;  // round up
                        answerType(F("fetch_start"));
                        return sendEvent(GH_DOWNLOAD, conn);
                    }
                }
#endif
                answerType(F("fetch_err"));
                return sendEvent(GH_DOWNLOAD_ERROR, conn);

            // upload
            case 5:
#ifndef GH_NO_FS
                if (!file_d && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_UPLOAD)) {
                    _fsmakedir(name);
                    file_u = GH_FS.open(name, "w");
                    if (file_u) {
                        fs_buffer = (char*)malloc(GH_UPL_CHUNK_SIZE + 10);
                        if (fs_buffer) {
                            fs_hub = hub;
                            fs_tmr = millis();
                            answerType(F("upload_start"));
                            sendEvent(GH_UPLOAD, conn);
                            return;
                        }
                    }
                }
#endif
                answerType(F("upload_err"));
                return sendEvent(GH_UPLOAD_ERROR, conn);

            // upload_chunk
            case 6:
#ifndef GH_NO_FS
                if (file_u && fs_hub == hub && fs_buffer) {
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
                return sendEvent(GH_UPLOAD_ERROR, conn);

            // ota
            case 7:
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA)
                if (!file_d && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_OTA)) {
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
                                fs_hub = hub;
                                ota_f = true;
                                fs_tmr = millis();
                                answerType(F("ota_start"));
                                return sendEvent(GH_OTA, conn);
                            }
                        }
                    }
                }
#endif
                answerType(F("ota_err"));
                return sendEvent(GH_OTA_ERROR, conn);

            // ota_chunk
            case 8:
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA)
                if (ota_f && fs_hub == hub && fs_buffer) {
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
                return sendEvent(GH_OTA_ERROR, conn);

            // ota_url
            case 9:
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA) && !defined(GH_NO_OTA_URL)
                if (!file_d && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_OTA_URL)) {
                    if (!strcmp_P(name, PSTR("flash"))) ota_url_fs = 0;
                    else if (!strcmp_P(name, PSTR("fs"))) ota_url_fs = 1;
                    ota_url = value;
                    answerType();
                    fs_state = GH_OTA_URL;
                    return sendEvent(GH_OTA_URL, conn);
                }
#endif
                answerErr(F("File busy"));
                return sendEvent(GH_OTA_URL, conn);

#endif
            default:
                clearFocus(conn);
                return sendEvent(GH_UNKNOWN, conn);
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

#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
        tickWS();
        tickHTTP();
#endif
#ifndef GH_NO_MQTT
        tickMQTT();
#endif

#ifndef GH_NO_FS
        if ((file_d || file_u || ota_f) && (uint16_t)millis() - fs_tmr >= (GH_CONN_TOUT * 1000)) {
            if (file_d) fs_state = GH_DOWNLOAD_ABORTED;
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
                    hub_ptr = &fs_hub;
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
                    file_d.close();
                    sendEvent(GH_DOWNLOAD_ABORTED, fs_hub.conn);
                    break;

                case GH_UPLOAD_CHUNK:
                    GH_B64toFile(file_u, fs_buffer);
                    hub_ptr = &fs_hub;
                    answerType(F("upload_next_chunk"));
                    fs_tmr = millis();
                    sendEvent(GH_UPLOAD_CHUNK, fs_hub.conn);
                    break;

                case GH_UPLOAD_FINISH:
                    GH_B64toFile(file_u, fs_buffer);
                    delete fs_buffer;
                    fs_buffer = nullptr;
                    file_u.close();
                    hub_ptr = &fs_hub;
                    answerType(F("upload_end"));
                    sendEvent(GH_UPLOAD_FINISH, fs_hub.conn);
                    break;

                case GH_UPLOAD_ABORTED:
                    file_u.close();
                    sendEvent(GH_UPLOAD_ABORTED, fs_hub.conn);
                    break;
#ifndef GH_NO_OTA
                case GH_OTA_CHUNK:
                    GH_B64toUpdate(fs_buffer);
                    hub_ptr = &fs_hub;
                    answerType(F("ota_next_chunk"));
                    fs_tmr = millis();
                    sendEvent(GH_OTA_CHUNK, fs_hub.conn);
                    break;

                case GH_OTA_FINISH:
                    GH_B64toUpdate(fs_buffer);
                    delete fs_buffer;
                    fs_buffer = nullptr;
                    ota_f = false;
                    reboot_f = GH_REB_OTA;
                    hub_ptr = &fs_hub;
                    if (Update.end(true)) answerType(F("ota_end"));
                    else answerType(F("ota_err"));
                    sendEvent(GH_OTA_FINISH, fs_hub.conn);
                    break;

                case GH_OTA_ABORTED:
                    Update.end();
                    ota_f = false;
                    sendEvent(GH_OTA_ABORTED, fs_hub.conn);
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
        reboot_f = GH_REB_OTA;
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
                    answer(*sptr, false);
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

        answ += F("'info':{'version':{");
        _jsStr(answ, F("Library"), GH_LIB_VERSION);
        if (version) _jsStr(answ, F("Firmware"), version);

        checkEndInfo(answ, GH_INFO_VERSION);
        answ += (F(",'net':{"));
#ifdef GH_ESP_BUILD
        _jsStr(answ, F("Mode"), WiFi.getMode() == WIFI_AP ? F("AP") : (WiFi.getMode() == WIFI_STA ? F("STA") : F("AP_STA")));
        _jsStr(answ, F("MAC"), WiFi.macAddress());
        _jsStr(answ, F("SSID"), WiFi.SSID());
        _jsStr(answ, F("RSSI"), String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + '%');
        _jsStr(answ, F("IP"), WiFi.localIP().toString());
        _jsStr(answ, F("AP_IP"), WiFi.softAPIP().toString());
#endif
        checkEndInfo(answ, GH_INFO_NETWORK);
        answ += (F(",'memory':{"));

#ifdef GH_ESP_BUILD
        _jsVal(answ, F("RAM"), String("[") + ESP.getFreeHeap() + ",0]");

#ifdef ESP8266
        FSInfo fs_info;
        GH_FS.info(fs_info);
        _jsVal(answ, F("Flash"), String("[") + fs_info.usedBytes + ',' + fs_info.totalBytes + "]");
#else
        _jsVal(answ, F("Flash"), String("[") + GH_FS.usedBytes() + ',' + GH_FS.totalBytes() + "]");
#endif

        _jsVal(answ, F("Sketch"), String("[") + ESP.getSketchSize() + ',' + ESP.getFreeSketchSpace() + "]");
#endif

        checkEndInfo(answ, GH_INFO_MEMORY);
        answ += (F(",'system':{"));
        _jsVal(answ, F("Uptime"), millis() / 1000ul);
#ifdef GH_ESP_BUILD
#ifdef ESP8266
        _jsStr(answ, F("Model"), F("ESP8266"));
#else
        _jsStr(answ, F("Model"), F("ESP32"));
#endif
        _jsVal(answ, F("CPU_MHz"), ESP.getCpuFreqMHz());
        _jsStr(answ, F("Flash_chip"), String(ESP.getFlashChipSize() / 1000.0, 1) + " kB");
#endif

        checkEndInfo(answ, GH_INFO_SYSTEM);
        answ += (F("}"));

        _jsEnd(answ);
        answer(answ);
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
        build.hub = *hub_ptr;
        bptr = &build;
        bool chunked = buf_size;

#ifdef GH_ESP_BUILD
        if (build.hub.conn == GH_WS || build.hub.conn == GH_MQTT) chunked = false;
#endif

        if (!chunked) {
            build.type = GH_BUILD_COUNT;
            build.count = 0;
            buf_mode = GH_COUNT;
            buf_count = 0;
            String count;
            sptr = &count;
            tab_width = 0;
            build_cb();
        }

        String answ;
        answ.reserve((chunked ? buf_size : buf_count) + 100);
        answ = F("\n{'controls':[");
        buf_mode = chunked ? GH_CHUNKED : GH_NORMAL;
        build.type = GH_BUILD_UI;
        build.count = 0;
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
        answer(answ);
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
        answer(answ);
    }
    void answerErr(FSTR err) {
        String answ;
        answ.reserve(50);
        _jsBegin(answ);
        _jsID(answ);
        _jsStr(answ, F("type"), F("ERR"));
        _jsStr(answ, F("text"), err, true);
        _jsEnd(answ);
        answer(answ);
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
        answ = F("\n{'fs':{'/':0,");
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
        answer(answ);
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
        _jsVal(answ, F("modules"), modules.mods, true);
        _jsEnd(answ);
        answer(answ, true);
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
        answ += F("'data':'");
        GH_fileToB64(file_d, answ);
        answ += '\'';
        _jsEnd(answ);
        answer(answ);
#endif
#endif
    }

    // ======================= ANSWER ========================
    void answer(String& answ, bool close = true) {
        if (!hub_ptr) return;
        if (hub_ptr->manual) {
            if (manual_cb) manual_cb(answ, hub_ptr->conn, false);
        } else {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
            if (hub_ptr->conn == GH_WS) answerWS(answ);
#endif
#ifndef GH_NO_MQTT
            if (hub_ptr->conn == GH_MQTT) answerMQTT(answ, hub_ptr->id);
#endif
#endif
        }
        if (close) hub_ptr = nullptr;
    }

    // ======================= SEND ========================
    void send(String& answ, bool broadcast = false) {
        for (int i = 0; i < GH_CONN_AMOUNT; i++) {
            if (manual_cb) manual_cb(answ, (GHconn_t)i, broadcast);
        }

#ifdef GH_ESP_BUILD
#ifndef GH_NO_WS
        if (focus_arr[GH_WS]) sendWS(answ);
#endif
#ifndef GH_NO_MQTT
        if ((focus_arr[GH_MQTT] || broadcast)) sendMQTT(answ);
#endif
#endif
    }

    // ========================== MISC ==========================
    void setFocus(GHconn_t conn) {
        focus_arr[conn] = GH_CONN_TOUT;
    }
    void clearFocus(GHconn_t conn) {
        focus_arr[conn] = 0;
        hub_ptr = nullptr;
    }

    // ========================== ADDER ==========================
    template <typename T>
    void _jsVal(String& s, FSTR key, T value, bool last = false) {
        s += '\'';
        s += key;
        s += F("':");
        s += value;
        if (!last) s += ',';
    }
    template <typename T>
    void _jsStr(String& s, FSTR key, T value, bool last = false) {
        s += '\'';
        s += key;
        s += F("':'");
        s += value;
        s += '\'';
        if (!last) s += ',';
    }
    void _jsID(String& s, bool last = false) {
        s += F("'id':'");
        s += id;
        s += '\'';
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

    void (*build_cb)() = nullptr;
    void (*info_cb)(GHinfo_t info) = nullptr;
    void (*cli_cb)(String& str) = nullptr;
    void (*manual_cb)(String& s, GHconn_t conn, bool broadcast) = nullptr;
    void (*event_cb)(GHevent_t state, GHconn_t conn) = nullptr;
    GHhub* hub_ptr = nullptr;

    bool running_f = 0;
    bool refresh_f = 0;
    bool upd_f = 0;

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

#ifdef GH_ESP_BUILD
    void (*reboot_cb)(GHreason_t r) = nullptr;
    bool auto_f = 0;
    GHreason_t reboot_f = GH_REB_NONE;

#ifndef GH_NO_FS
#ifndef GH_NO_OTA_URL
    String ota_url;
    bool ota_url_f = 0;
    bool ota_url_fs = 0;
#endif
    bool fs_mounted = 0;
    GHhub fs_hub;
    GHevent_t fs_state = GH_IDLE;
    char* fs_buffer = nullptr;
    File file_d, file_u;
    bool ota_f = false;
    uint16_t dwn_chunk_count = 0;
    uint16_t dwn_chunk_amount = 0;
    uint16_t fs_tmr = 0;
#endif
#endif
};
#endif