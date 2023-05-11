#ifndef _GyverHUB_h
#define _GyverHUB_h

#include <Arduino.h>
#include <Stamp.h>

#include "builder.h"
#include "config.h"
#include "macro.h"
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
#else
#ifndef GH_NO_OTA
#ifndef GH_NO_OTA_URL
#include <HTTPUpdate.h>
#endif
#include <Update.h>
#endif
#include <WiFi.h>
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
// #include "http.h"

#endif

// ========================== CLASS ==========================
#ifdef GH_ESP_BUILD
class GyverHUB : public HubBuilder, public HubStream, public HubHTTP, public HubMQTT, public HubWS {
#else
class GyverHUB : public HubBuilder, public HubStream {
#endif
   public:
    // ========================== CONSTRUCT ==========================

    // настроить префикс, название и иконку. Опционально задать свой ID устройства (для esp он генерируется автоматически)
    GyverHUB(const char* prefix = "", const char* name = "", const char* icon = "", uint32_t id = 0) {
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
#ifndef GH_NO_OTA_URL

#endif

#ifdef GH_ESP_BUILD
#ifndef GH_NO_LOCAL
        if (modules.read(GH_MOD_LOCAL)) beginHTTP();
        if (modules.read(GH_MOD_LOCAL)) beginWS();
#endif
#ifndef GH_NO_MQTT
        if (modules.read(GH_MOD_MQTT)) beginMQTT();
#endif
#ifndef GH_NO_FS
        GH_FS.begin();
#endif
#endif
        running_f = true;
        setStatus(GH_START, GH_SYSTEM);
    }

    // остановить
    void end() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_LOCAL
        endHTTP();
        endWS();
#endif
#ifndef GH_NO_MQTT
        endMQTT();
#endif
#endif
        running_f = false;
        setStatus(GH_STOP, GH_SYSTEM);
    }

    // установить версию прошивки для отображения в Info и OTA
    void setVersion(const char* v) {
        version = v;
    }

    // установить размер буфера строки для сборки интерфейса в режиме MANUAL и STREAM
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
    void onManual(void (*handler)(String& s)) {
        manual_cb = *handler;
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
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), F("print"));
        _jsStr(answ, F("text"), s);
        if (color != GH_DEFAULT) _jsVal(answ, F("color"), color);
        answ[answ.length() - 1] = '}';
        answ += '\n';
        send(answ);
    }

    // ========================== STATUS ==========================

    // подключить обработчик изменения статуса
    void onStatus(void (*handler)(GHstatus status)) {
        status_cb = *handler;
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

    // обновить веб-интерфейс. Вызывать внутри обработчика build
    void refresh() {
        send_f = true;
    }

    // true - если билдер вызван для set или read операций
    bool buildRead() {
        return (bptr && (bptr->type == GH_BUILD_ACTION || bptr->type == GH_BUILD_READ));
    }

    // вернёт имя компонента в текущем действии
    const char* actionName() {
        return (bptr) ? bptr->action.name : nullptr;
    }

    // вернёт значение компонента в текущем действии
    const char* actionValue() {
        return (bptr) ? bptr->action.value : nullptr;
    }

    // ========================= NOTIF ==========================

    // отправить уведомление
    void sendPush(const String& text) {
        if (!running_f) return;
        String answ;
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), F("push"));
        _jsStr(answ, F("text"), text);
        answ[answ.length() - 1] = '}';
        answ += '\n';
        send(answ, true);
    }

    // ========================= UPDATE ==========================

    // отправить update вручную с указанием значения
    void sendUpdate(const String& name, const String& value) {
        if (!running_f) return;
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
        if (!running_f || !build_cb || bptr) return;
        GHbuild build(GH_BUILD_READ);
        bptr = &build;

        String answ;
        sptr = &answ;
        _updateBegin(answ);

        char* str = (char*)name.c_str();
        char* p = str;
        GHsplitter(NULL);
        while ((p = GHsplitter(str)) != NULL) {
            build.type = GH_BUILD_READ;
            build.action.name = p;
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
        answ += '}';
        answ += '\n';
        send(answ);
    }
    void _updateBegin(String& answ) {
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), F("update"));
        answ += F("'updates':{");
    }

    // ======================== SEND GET =========================

    // автоматически отправлять новое состояние на get-топик при изменении через set (умолч. false)
    void sendGetAuto(bool v) {
#ifdef GH_ESP_BUILD
        auto_f = v;
#endif
    }

    // отправить имя-значение на get-топик (MQTT)
    void sendGet(const String& name, const String& value) {
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
    void sendGet(const String& name) {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
        if (!running_f || !build_cb || bptr) return;
        GHbuild build(GH_BUILD_READ);
        bptr = &build;

        String value;
        sptr = &value;

        char* str = (char*)name.c_str();
        char* p = str;
        GHsplitter(NULL);
        while ((p = GHsplitter(str)) != NULL) {
            build.type = GH_BUILD_READ;
            build.action.name = p;
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
    void parse(char* url, GHconn_t conn = GH_MANUAL) {
        if (!running_f) return;
        char* eq = strchr(url, '=');
        char val[1] = "";
        if (eq) *eq = 0;
        parse(url, eq ? (eq + 1) : val, conn);
        if (eq) *eq = '=';
    }

    // парсить строку вида PREFIX/ID/HUB_ID/CMD/NAME с отдельным value
    void parse(char* url, char* value, GHconn_t conn = GH_MANUAL) {
#if defined(GH_ESP_BUILD) && !defined(GH_NO_FS) && !defined(GH_NO_OTA) && !defined(GH_NO_OTA_URL)
        if (ota_url_f) return;
#endif
        if (!running_f) return;
        if (conn == GH_MQTT && !modules.read(GH_MOD_MQTT)) return;
        if (conn == GH_WS && !modules.read(GH_MOD_LOCAL)) return;
        if (conn == GH_MANUAL && !modules.read(GH_MOD_MANUAL)) return;
        if (conn == GH_STREAM && !modules.read(GH_MOD_STREAM)) return;

        if (strncmp(url, prefix, strlen(prefix))) return setStatus(GH_UNKNOWN, conn);

        if (!strcmp(url, prefix)) {  // == prefix
            GHhub hub(conn, value);
            hub_ptr = &hub;
            answerDiscover(true);
            return setStatus(GH_DISCOVER_ALL, conn);
        }

        GHparser<5> p(url);

        if (strcmp(p.str[1], id)) return;  // wrong id

        if (p.size == 2) {
            GHhub hub(conn, value);
            hub_ptr = &hub;
            answerDiscover();
            return setStatus(GH_DISCOVER, conn);
        }

        if (p.size == 3) return setStatus(GH_UNKNOWN, conn);

        GHhub hub(conn, p.str[2]);
        hub_ptr = &hub;

        if (p.size == 4) {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_MQTT
            // MQTT HOOK
            if (conn == GH_MQTT && build_cb) {
                if (!strcmp_P(p.str[2], PSTR("read"))) {
                    if (modules.read(GH_MOD_READ)) sendGet(p.str[3]);
                    hub_ptr = nullptr;
                    return setStatus(GH_READ_HOOK, conn);
                } else if (!strcmp_P(p.str[2], PSTR("set"))) {
                    if (modules.read(GH_MOD_SET)) {
                        GHbuild build(GH_BUILD_ACTION, GH_ACTION_SET, p.str[3], value, hub);
                        bptr = &build;
                        build_cb();
                        bptr = nullptr;
                        hub_ptr = nullptr;
                        if (auto_f) sendGet(p.str[3], value);
                    }
                    return setStatus(GH_SET_HOOK, conn);
                }
            }
#endif
#endif
            setFocus(conn);

            switch (GH_getCmd(p.str[3])) {
                case 0:  // focus
                    answerUI();
                    return setStatus(GH_FOCUS, conn);

                case 1:  // ping
                    answerType();
                    return setStatus(GH_PING, conn);

                case 2:  // unfocus
                    clearFocus(conn);
                    return setStatus(GH_UNFOCUS, conn);

#ifdef GH_ESP_BUILD
                case 3:  // info
#ifndef GH_NO_INFO
                    if (modules.read(GH_MOD_INFO)) answerInfo();
                    else answerType(F("ERR"));
#else
                    answerType(F("ERR"));
#endif
                    return setStatus(GH_INFO, conn);

                case 4:  // fsbr
#ifndef GH_NO_FS
                    if (modules.read(GH_MOD_FSBR)) answerFsbr();
                    else answerType(F("ERR"));
#else
                    answerType(F("ERR"));
#endif
                    return setStatus(GH_FSBR, conn);

                case 5:  // reboot
                    if (modules.read(GH_MOD_REBOOT)) {
                        reboot_f = GH_REB_BUTTON;
                        answerType();
                    } else answerType(F("ERR"));
                    return setStatus(GH_REBOOT, conn);

                case 6:  // fetch_chunk
#ifndef GH_NO_FS
                    fs_tmr = millis();
                    if (!file_d || fs_hub != hub || !modules.read(GH_MOD_DOWNLOAD)) {
                        answerType(F("fetch_err"));
                        return setStatus(GH_DOWNLOAD_ERROR, conn);
                    } else {
                        answerChunk();
                        dwn_chunk_count++;
                        if (dwn_chunk_count >= dwn_chunk_amount) {
                            file_d.close();
                            return setStatus(GH_DOWNLOAD_FINISH, conn);
                        }
                        return setStatus(GH_DOWNLOAD_CHUNK, conn);
                    }
#endif
                    break;
#endif
                default:  // unknown
                    clearFocus(conn);
                    return setStatus(GH_UNKNOWN, conn);
            }
            return;
        }

        // p.size == 5
        char* name = p.str[4];
        switch (GH_getCmdN(p.str[3])) {
            // set
            case 0:
                if (!build_cb || !modules.read(GH_MOD_SET)) {
                    answerType();
                } else {
                    GHbuild build(GH_BUILD_ACTION, GH_ACTION_SET, name, value, hub);
                    bptr = &build;
                    send_f = 0;
                    build_cb();
                    bptr = nullptr;
#ifdef GH_ESP_BUILD
                    if (auto_f) sendGet(name, value);
#endif
                    if (send_f) answerUI();
                    else answerType();
                }
                return setStatus(GH_SET, conn);

            // click
            case 1:
                if (!build_cb || !modules.read(GH_MOD_CLICK)) {
                    answerType();
                } else {
                    GHbuild build(GH_BUILD_ACTION, (value[0] == '1') ? GH_ACTION_PRESS : GH_ACTION_RELEASE, name, 0, hub);
                    send_f = 0;
                    bptr = &build;
                    build_cb();
                    bptr = nullptr;
                    if (send_f) answerUI();
                    else answerType();
                }
                return setStatus(GH_CLICK, conn);

            // cli
            case 2:
                answerType();
                if (cli_cb) {
                    String str(value);
                    cli_cb(str);
                }
                return setStatus(GH_CLI, conn);

#ifdef GH_ESP_BUILD
            // delete
            case 3:
#ifndef GH_NO_FS
                if (modules.read(GH_MOD_DELETE) && GH_FS.remove(name)) answerFsbr();
                else answerType(F("ERR"));
#else
                answerType(F("ERR"));
#endif
                return setStatus(GH_DELETE, conn);

            // rename
            case 4:
#ifndef GH_NO_FS
                if (modules.read(GH_MOD_RENAME) && GH_FS.rename(name, value)) answerFsbr();
                else answerType(F("ERR"));
#else
                answerType(F("ERR"));
#endif
                return setStatus(GH_RENAME, conn);

            // fetch
            case 5:
#ifndef GH_NO_FS
                if (!file_d && !file_u && !ota_f && modules.read(GH_MOD_DOWNLOAD)) {
                    file_d = GH_FS.open(name, "r");
                    if (file_d) {
                        fs_hub = hub;
                        fs_tmr = millis();
                        dwn_chunk_count = 0;
                        dwn_chunk_amount = (file_d.size() + GH_DOWN_CHUNK_SIZE - 1) / GH_DOWN_CHUNK_SIZE;  // round up
                        answerType(F("fetch_start"));
                        return setStatus(GH_DOWNLOAD, conn);
                    }
                }
#endif
                answerType(F("fetch_err"));
                return setStatus(GH_DOWNLOAD_ERROR, conn);

            // upload
            case 6:
#ifndef GH_NO_FS
                if (!file_d && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_UPLOAD)) {
                    file_u = GH_FS.open(name, "w");
                    if (file_u) {
                        fs_buffer = (char*)malloc(GH_UPL_CHUNK_SIZE + 10);
                        if (fs_buffer) {
                            fs_hub = hub;
                            fs_tmr = millis();
                            answerType(F("upload_start"));
                            setStatus(GH_UPLOAD, conn);
                            return;
                        }
                    }
                }
#endif
                answerType(F("upload_err"));
                return setStatus(GH_UPLOAD_ERROR, conn);

            // upload_chunk
            case 7:
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
                return setStatus(GH_UPLOAD_ERROR, conn);

            // ota
            case 8:
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA)
                if (!file_d && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_OTA)) {
                    int ota_type = 0;
                    if (!strcmp_P(name, PSTR("flash"))) ota_type = 1;
                    else if (!strcmp_P(name, PSTR("fs"))) ota_type = 2;

                    if (ota_type) {
                        size_t ota_size;
                        if (ota_type == 1) {
                            ota_type = U_FLASH;
                            ota_size = (size_t)((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
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
                                return setStatus(GH_OTA, conn);
                            }
                        }
                    }
                }
#endif
                answerType(F("ota_err"));
                return setStatus(GH_OTA_ERROR, conn);

            // ota_chunk
            case 9:
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
                return setStatus(GH_OTA_ERROR, conn);

            // ota_url
            case 10:
#if !defined(GH_NO_FS) && !defined(GH_NO_OTA) && !defined(GH_NO_OTA_URL)
                if (!file_d && !file_u && !ota_f && !fs_buffer && modules.read(GH_MOD_OTA_URL)) {
                    ota_url = value;
                    answerType();
                    fs_state = GH_OTA_URL;
                    return setStatus(GH_OTA_URL, conn);
                }
#endif
                answerType(F("ERR"));
                return setStatus(GH_OTA_URL, conn);

#endif
            default:
                clearFocus(conn);
                return setStatus(GH_UNKNOWN, conn);
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
        if (modules.read(GH_MOD_STREAM)) tickStream();
#endif
#ifdef GH_ESP_BUILD
#ifndef GH_NO_LOCAL
        if (modules.read(GH_MOD_LOCAL)) tickHTTP();
        if (modules.read(GH_MOD_LOCAL)) tickWS();
#endif
#ifndef GH_NO_MQTT
        if (modules.read(GH_MOD_MQTT)) tickMQTT();
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
                    client.setInsecure();
                    ok = ESPhttpUpdate.update(client, ota_url);
#else
                    httpUpdate.rebootOnUpdate(false);
                    WiFiClientSecure client;
                    client.setInsecure();
                    ok = httpUpdate.update(client, ota_url);
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
                    setStatus(GH_DOWNLOAD_ABORTED, fs_hub.conn);
                    break;

                case GH_UPLOAD_CHUNK:
                    B64toFile(file_u, fs_buffer);
                    hub_ptr = &fs_hub;
                    answerType(F("upload_next_chunk"));
                    fs_tmr = millis();
                    setStatus(GH_UPLOAD_CHUNK, fs_hub.conn);
                    break;

                case GH_UPLOAD_FINISH:
                    B64toFile(file_u, fs_buffer);
                    delete fs_buffer;
                    fs_buffer = nullptr;
                    file_u.close();
                    hub_ptr = &fs_hub;
                    answerType(F("upload_end"));
                    setStatus(GH_UPLOAD_FINISH, fs_hub.conn);
                    break;

                case GH_UPLOAD_ABORTED:
                    file_u.close();
                    setStatus(GH_UPLOAD_ABORTED, fs_hub.conn);
                    break;
#ifndef GH_NO_OTA
                case GH_OTA_CHUNK:
                    B64toUpdate(fs_buffer);
                    hub_ptr = &fs_hub;
                    answerType(F("ota_next_chunk"));
                    fs_tmr = millis();
                    setStatus(GH_OTA_CHUNK, fs_hub.conn);
                    break;

                case GH_OTA_FINISH:
                    B64toUpdate(fs_buffer);
                    delete fs_buffer;
                    fs_buffer = nullptr;
                    ota_f = false;
                    reboot_f = GH_REB_OTA;
                    hub_ptr = &fs_hub;
                    if (Update.end(true)) answerType(F("ota_end"));
                    else answerType(F("ota_err"));
                    setStatus(GH_OTA_FINISH, fs_hub.conn);
                    break;

                case GH_OTA_ABORTED:
                    Update.end();
                    ota_f = false;
                    setStatus(GH_OTA_ABORTED, fs_hub.conn);
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
    const char* getPrefix() {
        return prefix;
    }
    const char* getID() {
        return id;
    }
    void setStatus(GHstate_t state, GHconn_t conn) {
        if (status_cb) status_cb((GHstatus){conn, state});
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
        if (!modules.read(GH_MOD_MQTT)) return;
        String topic(prefix);
        topic += F("/hub/");
        topic += id;
        topic += F("/status");
        sendMQTT(topic, mode);
#endif
#endif
    }

    // ======================= INFO ========================
    void answerInfo() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_INFO
        String answ;
        answ.reserve(250);
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), F("info"));
        answ += F("'info':[");
        _jsArr(answ, WiFi.getMode() == WIFI_AP ? F("AP") : (WiFi.getMode() == WIFI_STA ? F("STA") : F("AP_STA")));
        _jsArr(answ, WiFi.SSID());
        _jsArr(answ, WiFi.localIP().toString());
        _jsArr(answ, WiFi.softAPIP().toString());
        _jsArr(answ, WiFi.macAddress());
        _jsArr(answ, "📶 " + String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + '%');
        _jsArr(answ, GH_uptime());
        _jsArr(answ, String(ESP.getFreeHeap() / 1000.0, 3) + " kB");
        _jsArr(answ, String(ESP.getSketchSize() / 1000.0, 1) + " kB (" + String(ESP.getFreeSketchSpace() / 1000.0, 1) + ")");
        _jsArr(answ, String(ESP.getFlashChipSize() / 1000.0, 1) + " kB");
        _jsArr(answ, String(ESP.getCpuFreqMHz()) + F(" MHz"));
        _jsArr(answ, version);
        _jsArr(answ, GH_VERSION);
        _jsArr(answ, id);
        answ[answ.length() - 1] = ']';  // ',' = ']'
        answ += '}';
        answ += '\n';
        answer(answ);
#endif
#endif
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
            buf_mode = GH_COUNT;
            buf_count = 0;
            String count;
            sptr = &count;
            build_cb();
        }

        String answ;
        answ.reserve((chunked ? buf_size : buf_count) + 100);
        answ = F("\n{'controls':[");
        buf_mode = chunked ? GH_CHUNKED : GH_NORMAL;
        build.type = GH_BUILD_UI;
        sptr = &answ;
        build_cb();
        sptr = nullptr;
        bptr = nullptr;

        if (answ[answ.length() - 1] == ',') answ[answ.length() - 1] = ']';  // ',' = ']'
        else answ += ']';
        answ += ',';
        _jsID(answ);
        _jsStr(answ, F("type"), F("ui"));
        answ[answ.length() - 1] = '}';  // ',' = '}'
        answ += '\n';
        answer(answ);
    }

    // ======================= TYPE ========================
    void answerType(FSTR type = nullptr) {
        if (!type) type = F("OK");
        String answ;
        answ.reserve(50);
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), type);
        answ[answ.length() - 1] = '}';  // ',' = '}'
        answ += '\n';
        answer(answ);
    }

    // ======================= FSBR ========================
    void answerFsbr() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
        uint16_t count = 0;
        String answ;
        answ.reserve(100);
        showFiles(answ, "/", GH_FS_DEPTH, &count);
        answ.reserve(count + 50);
        answ = F("\n{'fs':{'/':0,");
        showFiles(answ, "/", GH_FS_DEPTH);
        answ[answ.length() - 1] = '}';  // ',' = '}'
        answ += ',';

        _jsID(answ);
        _jsStr(answ, F("type"), F("fsbr"));

#ifdef ESP8266
        FSInfo fs_info;
        GH_FS.info(fs_info);
        _jsVal(answ, F("total"), fs_info.totalBytes);
        _jsVal(answ, F("used"), fs_info.usedBytes);
#else
        _jsVal(answ, F("total"), GH_FS.totalBytes());
        _jsVal(answ, F("used"), GH_FS.usedBytes());
#endif
        answ[answ.length() - 1] = '}';  // ',' = '}'
        answ += '\n';
        answer(answ);
#endif
#else
        answerType(F("ERR"));
#endif
    }

    // ======================= DISCOVER ========================
    String answerDiscover(bool broadcast = false) {
        uint32_t hash = 0;
        if (PIN > 999) {
            char pin_s[11];
            ultoa(PIN, pin_s, 10);
            uint16_t len = strlen(pin_s);
            for (uint16_t i = 0; i < len; i++) {
                hash = (((uint32_t)hash << 5) - hash) + pin_s[i];
            }
        }

        String ip(F("unset"));
#ifdef GH_ESP_BUILD
        if (WiFi.getMode() == WIFI_AP) ip = WiFi.softAPIP().toString();
        else if (WiFi.getMode() == WIFI_STA) ip = WiFi.localIP().toString();
        else if (WiFi.getMode() == WIFI_AP_STA) {
            if (WiFi.status() == WL_CONNECTED) ip = WiFi.localIP().toString();
            else ip = WiFi.softAPIP().toString();
        }
#endif

        String answ;
        answ.reserve(120);
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), F("discover"));
        _jsStr(answ, F("name"), name);
        _jsStr(answ, F("icon"), icon);
        _jsStr(answ, F("ip"), ip);
        _jsVal(answ, F("PIN"), hash);
        _jsStr(answ, F("version"), version);
        _jsVal(answ, F("max_upl"), GH_UPL_CHUNK_SIZE);
        answ[answ.length() - 1] = '}';  // ',' = '}'
        answ += '\n';
        answer(answ, true, broadcast);
        return answ;
    }

    // ======================= CHUNK ========================
    void answerChunk() {
#ifdef GH_ESP_BUILD
#ifndef GH_NO_FS
        String answ;
        answ.reserve(GH_DOWN_CHUNK_SIZE + 100);
        answ += '\n';
        answ += '{';
        _jsID(answ);
        _jsStr(answ, F("type"), F("fetch_next_chunk"));
        _jsVal(answ, F("chunk"), dwn_chunk_count);
        _jsVal(answ, F("amount"), dwn_chunk_amount);
        answ += F("'data':'");
        fileToB64(file_d, answ);
        answ += F("'}\n");
        answer(answ);
#endif
#endif
    }

    // ======================= ANSWER ========================
    void answer(String& answ, bool close = true, bool broadcast = false) {
        if (!hub_ptr) return;
        switch (hub_ptr->conn) {
            case GH_STREAM:
#ifndef GH_NO_STREAM
                if (modules.read(GH_MOD_STREAM)) sendStream(answ);
#endif
                break;
            case GH_MANUAL:
                if (manual_cb && modules.read(GH_MOD_MANUAL)) manual_cb(answ);
                break;
#ifdef GH_ESP_BUILD
            case GH_WS:
#ifndef GH_NO_LOCAL
                if (modules.read(GH_MOD_LOCAL)) answerWS(answ);
#endif
                break;
            case GH_MQTT:
#ifndef GH_NO_MQTT
                if (modules.read(GH_MOD_MQTT)) answerMQTT(hub_ptr->id, answ, broadcast);
#endif
                break;
#endif
            default:
                break;
        }
        if (close) hub_ptr = nullptr;
    }

    // ======================= SEND ========================
    void send(String& answ, bool broadcast = false) {
        if (modules.read(GH_MOD_MANUAL) && focus_arr[0]) {  // GH_MANUAL
            if (manual_cb) manual_cb(answ);
        }
        if (modules.read(GH_MOD_STREAM) && focus_arr[1]) {  // GH_STREAM
#ifndef GH_NO_STREAM
            sendStream(answ);
#endif
        }
#ifdef GH_ESP_BUILD
#ifndef GH_NO_LOCAL
        if (modules.read(GH_MOD_LOCAL) && focus_arr[2]) {  // GH_WS
            sendWS(answ);
        }
#endif
#ifndef GH_NO_MQTT
        if (modules.read(GH_MOD_MQTT) && (focus_arr[3] || broadcast)) {  // GH_MQTT
            sendMQTT(answ);                                              // broadcast!
        }
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
    void _jsVal(String& s, FSTR key, uint32_t value) {
        s += '\'';
        s += key;
        s += F("':");
        s += value;
        s += F(",");
    }
    template <typename T>
    void _jsStr(String& s, FSTR key, T value) {
        s += '\'';
        s += key;
        s += F("':'");
        s += value;
        s += F("',");
    }
    void _jsArr(String& s, const String& value) {
        s += '\'';
        s += value;
        s += F("',");
    }
    void _jsID(String& s) {
        s += F("'id':'");
        s += id;
        s += F("',");
    }

    // ========================== VARS ==========================
    const char* prefix = nullptr;
    const char* name = nullptr;
    const char* icon = nullptr;
    const char* version = nullptr;
    uint32_t PIN = 0;
    char id[9];

    void (*build_cb)() = nullptr;
    void (*cli_cb)(String& str) = nullptr;
    void (*manual_cb)(String& s) = nullptr;
    void (*status_cb)(GHstatus status) = nullptr;
    GHhub* hub_ptr = nullptr;

    bool running_f = 0;
    bool send_f = 0;

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
#endif
    GHhub fs_hub;
    GHstate_t fs_state = GH_IDLE;
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