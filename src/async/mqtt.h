#pragma once
#include "../config.hpp"
#include "../macro.hpp"

#ifdef GH_ESP_BUILD
#ifdef GH_NO_MQTT
class HubMQTT {
   public:
    void setupMQTT(const char* host, uint16_t port, const char* login = nullptr, const char* pass = nullptr, uint8_t nqos = 0, bool nret = 0) {}
};
#else

#include <Arduino.h>
#include <AsyncMqttClient.h>

#include "../utils/stats.h"

class HubMQTT {
    // ============ PUBLIC =============
   public:
    // настроить MQTT (хост брокера, порт, логин, пароль, QoS, retained)
    void setupMQTT(const char* host, uint16_t port, const char* login = nullptr, const char* pass = nullptr, uint8_t nqos = 0, bool nret = 0) {
        if (!strlen(host)) return;
        mqtt.setServer(host, port);
        _setupMQTT(login, pass, nqos, nret);
    }
    void setupMQTT(IPAddress ip, uint16_t port, const char* login = nullptr, const char* pass = nullptr, uint8_t nqos = 0, bool nret = 0) {
        mqtt.setServer(ip, port);
        _setupMQTT(login, pass, nqos, nret);
    }

    // MQTT подключен
    bool online() {
        return mqtt.connected();
    }

    // ============ PROTECTED =============
   protected:
    virtual void parse(char* url, char* value, GHconn_t from) = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void sendEvent(GHevent_t state, GHconn_t from) = 0;

    void beginMQTT() {
        mqtt.onConnect([this](GH_UNUSED bool pres) {
            String sub_topic(getPrefix());
            mqtt.subscribe(sub_topic.c_str(), qos);

            sub_topic += '/';
            sub_topic += getID();
            sub_topic += "/#";
            mqtt.subscribe(sub_topic.c_str(), qos);

            String status(getPrefix());
            status += F("/hub/");
            status += getID();
            status += F("/status");
            String offline(F("offline"));
            mqtt.setWill(status.c_str(), qos, ret, offline.c_str());

            String online(F("online"));
            sendMQTT(status, online);
            sendEvent(GH_CONNECTED, GH_MQTT);
            mqtt_tmr = millis();
        });

        mqtt.onDisconnect([this](GH_UNUSED AsyncMqttClientDisconnectReason reason) {
            String m_id("DEV-");
            m_id += String(random(0xffffff), HEX);
            mqtt.setClientId(m_id.c_str());
            sendEvent(GH_DISCONNECTED, GH_MQTT);
            mqtt_tmr = millis();
        });

        mqtt.onMessage([this](char* topic, char* data, GH_UNUSED AsyncMqttClientMessageProperties prop, size_t len, GH_UNUSED size_t index, GH_UNUSED size_t total) {
            char buf[len + 1];
            memcpy(buf, data, len);
            buf[len] = 0;
            parse(topic, buf, GH_MQTT);
        });
    }

    void endMQTT() {
        mqtt.disconnect();
    }

    void tickMQTT() {
        if (mq_configured && !mqtt.connected() && (!mqtt_tmr || millis() - mqtt_tmr > GH_MQTT_RECONNECT)) {
            mqtt_tmr = millis();
            sendEvent(GH_CONNECTING, GH_MQTT);
            mqtt.connect();
        }
    }

    void sendMQTT(const String& topic, const String& msg) {
        if (mqtt.connected()) mqtt.publish(topic.c_str(), qos, ret, msg.c_str(), msg.length());
    }

    void sendMQTT(const String& msg) {
        String topic(getPrefix());
        topic += F("/hub");
        sendMQTT(topic, msg);
    }

    void answerMQTT(const String& msg, const char* hubID) {
        String topic(getPrefix());
        topic += F("/hub/");
        topic += hubID;
        topic += '/';
        topic += getID();
        sendMQTT(topic, msg);
    }

    // ============ PRIVATE =============
   private:
    void _setupMQTT(const char* login, const char* pass, uint8_t nqos, bool nret) {
        if (mqtt.connected()) mqtt.disconnect();
        mqtt.setCredentials(login, pass);
        qos = nqos;
        ret = nret;
        mq_configured = true;
    }

    AsyncMqttClient mqtt;
    bool mq_configured = false;
    uint32_t mqtt_tmr = 0;
    uint8_t qos = 0;
    bool ret = 0;
};
#endif
#endif