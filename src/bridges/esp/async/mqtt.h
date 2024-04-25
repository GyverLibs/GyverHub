#pragma once
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <GyverHub.h>
#include <StringUtils.h>

#include "core/bridge.h"

// заметки: нужно делать буфер хаба поменьше. Также больше 4-5 пакетов нормально не отправляет!

/*
lib_deps =
    heman/AsyncMqttClient-esphome
    esphome/ESPAsyncTCP-esphome     ;(esp8266)
    esphome/AsyncTCP-esphome        ;(esp32)
*/

namespace gh {

class BridgeMqtt : public gh::Bridge {
   public:
    BridgeMqtt(GyverHub* hub) : Bridge(hub, Connection::MQTT, GyverHub::parseHook) {}

    // настроить MQTT (хост брокера, порт, логин, пароль, QoS, retained)
    void config(const String& host, uint16_t port, const String& login = "", const String& pass = "", uint8_t qos = 2, bool ret = 0) {
        _host = host;
        mqtt.setServer(_host.c_str(), port);
        _config(login, pass, qos, ret);
    }

    // настроить MQTT (хост брокера, порт, логин, пароль, QoS, retained)
    void config(const IPAddress& ip, uint16_t port, const String& login = "", const String& pass = "", uint8_t qos = 2, bool ret = 0) {
        mqtt.setServer(ip, port);
        _config(login, pass, qos, ret);
    }

    // MQTT подключен
    bool online() {
        return mqtt.connected();
    }

    void begin() {
        mqtt.onConnect([this](bool sessionPresent) {
            mqtt.subscribe(((GyverHub*)_hub)->topicDiscover().c_str(), _qos);
            mqtt.subscribe(((GyverHub*)_hub)->topicHub().c_str(), _qos);
        });

        mqtt.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
            if (_state) _reconnect.startTimeout(GH_MQTT_RECONNECT);
        });

        mqtt.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
            parse(sutil::AnyText(topic), sutil::AnyText(payload, len));
        });

        mqtt.connect();
        _state = 1;
    }
    void end() {
        mqtt.disconnect();
        _state = 0;
    }

    void tick() {
        if (_reconnect) mqtt.connect();
    }

    void send(BridgeData& data) {
        mqtt.publish(data.topic.c_str(), _qos, _ret, data.text.str(), data.text.length());
    }

    AsyncMqttClient mqtt;

   private:
    gh::Timer _reconnect;
    bool _state = 0;
    uint8_t _qos = 2;
    bool _ret = 0;
    String _host;
    String _login;
    String _pass;

    void _config(const String& login, const String& pass, uint8_t qos, bool ret) {
        _login = login;
        _pass = pass;
        if (_login.length()) mqtt.setCredentials(_login.c_str(), _pass.c_str());
        else mqtt.setCredentials(nullptr, nullptr);
        _qos = qos;
        _ret = ret;
    }
};

}  // namespace gh