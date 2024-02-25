#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"

#ifdef GH_ESP_BUILD
#include <PubSubClient.h>
#include <WiFiClient.h>

#include "core/core_class.h"
#include "core/hooks.h"
#include "core/hub_class.h"
#include "core/types.h"

namespace ghc {

class HubMQTT : public gh::Bridge {
    friend class HubCore;
    friend class ::GyverHub;

   public:
    // настроить MQTT (хост брокера, порт, логин, пароль, QoS, retained)
    void config(const String& host, uint16_t port, const String& login = "", const String& pass = "", uint8_t qos = 0, bool ret = 0) {
        if (!host.length()) return;
        mq_host = host;
        mqtt.setServer(mq_host.c_str(), port);
        _config(login, pass, qos, ret);
    }

    // настроить MQTT (хост брокера, порт, логин, пароль, QoS, retained)
    void config(IPAddress ip, uint16_t port, const String& login = "", const String& pass = "", uint8_t qos = 0, bool ret = 0) {
        mqtt.setServer(ip, port);
        _config(login, pass, qos, ret);
    }

    // MQTT подключен
    bool online() {
        return mqtt.connected();
    }

    // ======================= PRIVATE =======================
   private:
    HubMQTT() {
        mqtt.setClient(mclient);
        mqtt.setBufferSize(GH_UPL_CHUNK_SIZE);
    }

    void begin() {
        mqtt.setCallback([this](char* topic, uint8_t* data, uint16_t len) {
            uint16_t tlen = strlen(topic);
            char topic_buf[tlen + 1];
            memcpy(topic_buf, topic, tlen);
            topic_buf[tlen] = 0;

            char data_buf[len + 1];
            memcpy(data_buf, data, len);
            data_buf[len] = 0;

            parse(GHTXT(topic_buf, tlen), GHTXT(data_buf, len));
        });
    }

    void end() {
        mqtt.disconnect();
    }

    void setup(void* hub, ParseHook hook, char* id, String* prefix) {
        gh::Bridge::config(hub, gh::Connection::MQTT, hook);
        this->id = id;
        this->prefix = prefix;
    }

    void tick() {
        if (mq_configured) {
            if (!mqtt.connected() && (!mqtt_tmr || millis() - mqtt_tmr > GH_MQTT_RECONNECT)) {
                mqtt_tmr = millis();
                _connect();
            }
            mqtt.loop();
        }
    }

    void send(gh::BridgeData& data) {
        if (!mqtt.connected()) return;
        mqtt.beginPublish(data.topic.c_str(), data.text.length(), ret);
        mqtt.print(data.text);
        mqtt.endPublish();
    }

    // ======================= PRIVATE =======================
    PubSubClient mqtt;
    WiFiClient mclient;
    bool mq_configured = false;
    uint32_t mqtt_tmr = 0;
    uint8_t qos = 0;
    bool ret = 0;
    String mq_host;
    String mq_login;
    String mq_pass;

    char* id;
    String* prefix;

    void _connect() {
        String m_id("hub_");
        m_id += String(random(0xffffff), HEX);
        bool ok = 0;

        String status(*prefix);
        status += F("/hub/");
        status += id;
        status += F("/status");

        String offline(F("offline"));

        if (mq_login.length()) ok = mqtt.connect(m_id.c_str(), mq_login.c_str(), mq_pass.c_str(), status.c_str(), qos, ret, offline.c_str());
        else ok = mqtt.connect(m_id.c_str(), status.c_str(), qos, ret, offline.c_str());

        if (ok) {
            String online(F("online"));
            mqtt.publish(status.c_str(), online.c_str());

            String sub_topic(*prefix);
            mqtt.subscribe(sub_topic.c_str(), qos);

            sub_topic += '/';
            sub_topic += id;
            sub_topic += "/#";
            mqtt.subscribe(sub_topic.c_str(), qos);
        }
    }
    void _config(const String& login, const String& pass, uint8_t nqos, bool nret) {
        qos = nqos;
        ret = nret;
        mq_login = login;
        mq_pass = pass;
        mq_configured = true;
    }
};

}  // namespace ghc

#endif