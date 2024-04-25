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
        if (client.connected()) client.disconnect();
        mq_host = host;
        client.setServer(mq_host.c_str(), port);
        _config(login, pass, qos, ret);
    }

    // настроить MQTT (хост брокера, порт, логин, пароль, QoS, retained)
    void config(const IPAddress& ip, uint16_t port, const String& login = "", const String& pass = "", uint8_t qos = 0, bool ret = 0) {
        client.setServer(ip, port);
        _config(login, pass, qos, ret);
    }

    // MQTT подключен
    bool online() {
        return client.connected();
    }

    PubSubClient client;

    // ======================= PRIVATE =======================
   private:
    HubMQTT() {
        client.setClient(mclient);
        client.setBufferSize(GH_UPL_CHUNK_SIZE);
    }

    void begin() {
        client.setCallback([this](char* topic, uint8_t* data, uint16_t len) {
            uint16_t tlen = strlen(topic);
            char* topic_buf = new char[tlen];
            if (!topic_buf) return;
            memcpy(topic_buf, topic, tlen);

            if (!len) {
                parse(GHTXT(topic_buf, tlen));
                delete[] topic_buf;
                return;
            }

            char* data_buf = new char[len];
            if (!data_buf) {
                delete[] topic_buf;
                return;
            }
            memcpy(data_buf, data, len);

            parse(GHTXT(topic_buf, tlen), GHTXT(data_buf, len));

            delete[] data_buf;
            delete[] topic_buf;
        });
    }

    void end() {
        client.disconnect();
    }

    void setup(void* hub, ParseHook hook, uint32_t* id, String* net) {
        gh::Bridge::config(hub, gh::Connection::MQTT, hook);
        this->id = id;
        this->net = net;
    }

    void tick() {
        if (mq_configured) {
            if (!client.connected() && (!mqtt_tmr || millis() - mqtt_tmr > GH_MQTT_RECONNECT)) {
                mqtt_tmr = millis();
                _connect();
            }
            client.loop();
        }
    }

    void send(gh::BridgeData& data) {
        if (!client.connected()) return;
        client.beginPublish(data.topic.c_str(), data.text.length(), ret);
        client.print(data.text);
        client.endPublish();
    }

    // ======================= PRIVATE =======================
    WiFiClient mclient;
    bool mq_configured = false;
    uint32_t mqtt_tmr = 0;
    uint8_t qos = 0;
    bool ret = 0;
    String mq_host;
    String mq_login;
    String mq_pass;

    uint32_t* id;
    String* net;

    void _connect() {
        String m_id("hub_");
        m_id += String(random(0xffffff), HEX);
        bool ok = 0;
        su::Value device_id(*id, HEX);

        String status(*net);
        status += F("/hub/");
        device_id.addString(status);
        status += F("/status");

        String offline(F("offline"));

        if (mq_login.length()) ok = client.connect(m_id.c_str(), mq_login.c_str(), mq_pass.c_str(), status.c_str(), qos, ret, offline.c_str());
        else ok = client.connect(m_id.c_str(), status.c_str(), qos, ret, offline.c_str());

        if (ok) {
            String online(F("online"));
            client.publish(status.c_str(), online.c_str());

            String sub_topic(*net);
            client.subscribe(sub_topic.c_str(), qos);

            sub_topic += '/';
            device_id.addString(sub_topic);
            sub_topic += "/#";
            client.subscribe(sub_topic.c_str(), qos);

            sub_topic = *net;
            sub_topic += F("/" GH_BROAD_ID_STR "/#");
            client.subscribe(sub_topic.c_str(), qos);
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