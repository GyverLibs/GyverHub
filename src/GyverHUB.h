#ifndef _GyverHUB_h
#define _GyverHUB_h

#include <Arduino.h>
#include <GyverPortal.h>
#include "components.h"

#ifndef GH_NO_MQTT
#include <PubSubClient.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#else
#include <WiFi.h>
#endif

#define GH_IDLE         0
#define GH_TCP_UNKNOWN  1
#define GH_TCP_FIND     2
#define GH_TCP_CLICK    3
#define GH_TCP_UPDATE   4
#define GH_MQ_CONNECTED 5
#define GH_MQ_ERROR     6
#define GH_MQ_UNKNOWN   7
#define GH_MQ_FIND      8
#define GH_MQ_CLICK     9
#define GH_MQ_UPDATE    10
#define GH_START        11
#define GH_STOP         12

class GyverHUB : public Components, public ArgParser {
public:
    // конструктор
    GyverHUB() : server(80) {}

    // конструктор, настроить сеть, название, иконку
    GyverHUB(const char* nID, const char* nname = "", const char* nicon = "") : server(80) {
        config(nID, nname, nicon);
    }

    // настроить девайс (сеть, название, иконку https://fontawesome.com/v5/cheatsheet/free/solid)
    void config(const char* nID, const char* nname = "", const char* nicon = "") {
        net_ID = nID;
        name = nname;
        icon = nicon;
        uint8_t mac[6];
        WiFi.macAddress(mac);
        ultoa(*((uint32_t*)(mac + 2)), dev_ID, HEX);    // ID это 8 цифр MAC адреса
    }

    // настроить TCP порт (умолч. 50000)
    void setupTCP(uint16_t port) {
        tcp_port = port;
        if (running_f) {
            server.stop();
            server.begin(tcp_port);
        }
    }

    // настроить MQTT (хост брокера, порт, логин, пароль)
    void setupMQTT(const char* mq_host = "", uint16_t port = 0, const char* nmq_login = nullptr, const char* nmq_pass = nullptr) {
        #ifndef GH_NO_MQTT
        mq_login = nmq_login;
        mq_pass = nmq_pass;
        mq_configured = (bool)mq_host;
        mq_conn = 0;
        if (mqtt.connected()) mqtt.disconnect();

        mqtt.setClient(mclient);
        mqtt.setServer(mq_host, port);
        mqtt.setCallback([this](__attribute__((unused)) char* topic, uint8_t* str, uint16_t len) {
            if (!mq_configured) return;             // этого вообще не могло произойти
            if (millis() - tcp_tmr < 2000) return;  // игнорируем, если был запрос по TCP
            str[len] = 0;

            String answ;
            uint8_t type = buildAnswer(answ, (char*)str);
            sendMQTT(answ);
            switch (type) {
            case 0: stat = GH_MQ_UNKNOWN; break;
            case 1: stat = GH_MQ_FIND; break;
            case 2:
            case 3: stat = GH_MQ_CLICK; break;
            case 4: stat = GH_MQ_UPDATE; break;
            }
        });
        #endif
    }
    
    // открывать веб-интерфейс в браузере по IP устройства по долгому клику
    void usePortal(bool v) {
        portal = v;
    }

    // начать работу
    void begin() {
        if (running_f) stop();
        server.begin(tcp_port);
        stat = GH_START;
        running_f = 1;
    }

    // остановить работу
    void stop() {
        if (running_f) {
            running_f = 0;
            server.stop();
            #ifndef GH_NO_MQTT
            mqtt.disconnect();
            #endif
        }
        stat = GH_STOP;
        mq_conn = 0;
    }

    // подключить функцию для сборки интерфейса
    void attachBuild(void (*handler)()) {
        build_cb = *handler;
    }

    // подключить функцию-обработчик действий с приложения
    void attach(void (*handler)()) {
        action_cb = *handler;
    }

    // тикер, вызывать в loop
    bool tick() {
        if (!running_f) return 0;
        stat = GH_IDLE;
        
        WiFiClient client = server.available(); // accept() in 3+
        if (client) {
            String req = client.readStringUntil('/');   // "GET /"
            req = client.readStringUntil('\r');
            while (client.available()) client.read();
            int end = req.indexOf(F(" HTTP/"));
            if (end < 0) return 1;
            req.remove(end, req.length() - end);
            
            String answ;
            uint8_t type = buildAnswer(answ, req);
            if (type) {
                client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
                "Access-Control-Allow-Origin:*\r\n"
                "Access-Control-Allow-Private-Network: true\r\n"
                "Access-Control-Allow-Methods:*\r\n\r\n"));
                client.print(answ);
            } else client.print(F("HTTP/1.1 404 Not Found\r\n"));
            
            switch (type) {
            case 0: stat = GH_TCP_UNKNOWN; break;
            case 1: stat = GH_TCP_FIND; break;
            case 2:
            case 3: stat = GH_TCP_CLICK; break;
            case 4: stat = GH_TCP_UPDATE; break;
            }
        }
        
        #ifndef GH_NO_MQTT
        if (mq_configured) {
            mq_conn = mqtt.connected();
            if (!mq_conn && millis() - mqtt_tmr > 5000) {
                stat = GH_MQ_ERROR;
                String m_id("GH-");
                m_id += String(random(0xffffff), HEX);
                bool conn = 0;
                if (mq_login) conn = mqtt.connect(m_id.c_str(), mq_login, mq_pass);
                else conn = mqtt.connect(m_id.c_str());

                if (conn) {
                    mqtt.subscribe(net_ID);
                    String id_topic(net_ID);
                    id_topic += '/';
                    id_topic += dev_ID;
                    mqtt.subscribe(id_topic.c_str());
                    stat = GH_MQ_CONNECTED;
                }
                mqtt_tmr = millis();
            }
            mqtt.loop();
        }
        #endif
        return 1;
    }

    // получить статус (см. коды статусов)
    uint8_t status() {
        return stat;
    }
    
    // проверка онлайн (mqtt подключен?)
    bool online() {
        #ifndef GH_NO_MQTT
        return mqtt.connected();
        #endif
        return 0;
    }
    
    // запущен ли?
    bool running() {
        return running_f;
    }
    
    // статус изменился
    bool statusChanged() {
        if (p_stat != stat) {
            p_stat = stat;
            if (stat) return 1;
        }
        return 0;
    }
    
    // отправить интерфейс
    void refresh() {
        send_f = 1;
    }

private:
    void buildFind(String& answ) {
        String ip;
        if (WiFi.getMode() == WIFI_AP) ip = WiFi.softAPIP().toString();
        else ip = WiFi.localIP().toString();
        String status;
        status.reserve(20);
        sptr = &answ;
        answ.reserve(200);
        answ += '{';
        
        if (build_cb) {
            sstat_p = &status;
            answ += F("'controls':[");
            build_cb();
            answ[answ.length() - 1] = ']';  // ',' = ']'
            answ += ',';
            sstat_p = nullptr;
        }
        
        addStr(F("net_id"), net_ID);
        addStr(F("dev_id"), dev_ID);
        addStr(F("type"), F("find"));
        if (status.length()) addStr(F("status"), status);
        addStr(F("ip"), ip);
        if (portal) addBool(F("portal"), portal);
        if (name) addStr(F("name"), name);
        if (icon) addStr(F("icon"), icon);
        addVal(F("rssi"), (constrain(2 * (WiFi.RSSI() + 100), 0, 100)));
        
        answ[answ.length() - 1] = '}';  // ',' = '}'
        sptr = nullptr;
    }
    
    void buildUpdate(String& answ, String& name) {
        sptr = &answ;
        answ += '{';
        
        addStr(F("net_id"), net_ID);
        addStr(F("dev_id"), dev_ID);
        addStr(F("type"), F("update"));
        answ += F("'updates':['");
        
        _answPtr = &answ;
        
        if (name.indexOf(',') < 0) {            // один компонент
            _updPtr = &name;
            if (action_cb) action_cb();
        } else {
            GP_parser n(name);                  // парсер
            _updPtr = &n.str;                   // указатель на имя (в парсинге)
            while (n.parse()) {                 // парсим
                if (action_cb) action_cb();
                answ += "','";
                yield();
            }
            answ.remove(answ.length() - 3);     // удаляем последний разделитель
        }
        
        _answPtr = nullptr;
        _updPtr = nullptr;
        
        answ += F("']}");
        sptr = nullptr;
    }
    
    void sendMQTT(const String& answ) {
        #ifndef GH_NO_MQTT
        String out_topic(net_ID);
        out_topic += F("_app");
        mqtt.beginPublish(out_topic.c_str(), answ.length(), 0);
        mqtt.write((byte*)answ.c_str(), answ.length());
        mqtt.endPublish();
        #endif
    }
    
    uint8_t buildAnswer(String& answ, const String& req) {
        uint8_t type = 0;
        if (req.startsWith(net_ID)) {                       // find
            buildFind(answ);
            return 1;
        } else if (req.startsWith("GH_click?")) type = 2;   // click
        else if (req.startsWith("GH_press?")) type = 3;     // press
        else if (req.startsWith("GH_update?")) type = 4;    // update
        
        int eq = req.indexOf('=');
        if (type && eq > 0) {
            String name = req.substring(req.indexOf('?') + 1, eq);
            String value;
            urldecode(req.substring(eq + 1, req.length()), value);
            _argNamePtr = &name;
            _argValPtr = &value;
            
            if (type != 4) {    // click/press
                send_f = 0;
                if (type == 2) click_f = 1;     // click
                else {                          // press
                    _holdF = value[0] - '0';
                    if (_holdF == 1) _hold = name;
                    else _hold = "";
                }
                if (action_cb) action_cb();
                click_f = 0;
                _holdF = 0;
                if (send_f) buildFind(answ);
                else answ = "OK";
                
            } else {            // update
                buildUpdate(answ, name);
            }
            
            _argNamePtr = nullptr;
            _argValPtr = nullptr;
        }
        return type;
    }

    // ArgParser
    const String arg(const String& n) {
        if (_argNamePtr && _argValPtr) {
            if (n.equals(*_argNamePtr)) return *_argValPtr;
        }
        return _GP_empty_str;
    }
    bool hasArg(const String& n) {
        return _argNamePtr ? (_argNamePtr -> equals(n)) : 0;
    }
    bool clickF() {
        return click_f;
    }
    int args() {
        return (bool)_argNamePtr;
    }
    

    // json build
    void addStr(const String & key, const String & value) {
        *sptr += '\'';
        *sptr += key;
        *sptr += "':'";
        *sptr += value;
        *sptr += "',";
    }
    void addBool(const String & key, bool value) {
        *sptr += '\'';
        *sptr += key;
        *sptr += "':";
        *sptr += value ? "true" : "false";
        *sptr += ',';
    }
    template <typename T>
    void addVal(const String & key, T value) {
        *sptr += '\'';
        *sptr += key;
        *sptr += "':";
        *sptr += value;
        *sptr += ',';
    }
    
    void urldecode(const String& s, String& dest) {
        dest.reserve(s.length());
        char c;
        for (uint16_t i = 0; i < s.length(); i++) {
            c = s[i];
            if (c != '%') dest += (c == '+') ? ' ' : c;
            else {
                c = s[++i];
                uint8_t v1 = c - ((c <= '9') ? 48 : ((c <= 'F') ? 55 : 87));
                c = s[++i];
                uint8_t v2 = c - ((c <= '9') ? 48 : ((c <= 'F') ? 55 : 87));
                dest += char(v2 | (v1 << 4));
            }
        }
    }

    // vars
    const char* name = nullptr;
    const char* icon = nullptr;
    const char* net_ID = nullptr;
    
    const char* mq_login = nullptr;
    const char* mq_pass = nullptr;

    uint16_t tcp_port = 50000;
    
    void (*build_cb)() = nullptr;
    void (*action_cb)() = nullptr;
    uint32_t tcp_tmr = 0, mqtt_tmr = 0;
    char dev_ID[9];
    bool portal = 0;
    uint8_t stat = GH_STOP, p_stat = GH_STOP;
    bool running_f = 0;
    bool mq_conn = 0;
    bool mq_configured = 0;
    bool send_f = 0;
    bool click_f = 0;

    WiFiServer server;
    #ifndef GH_NO_MQTT
    WiFiClient mclient;
    PubSubClient mqtt;
    #endif
};
#endif
