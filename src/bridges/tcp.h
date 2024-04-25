#pragma once
#include <Arduino.h>
#include <GyverHub.h>
#include <StringUtils.h>

#ifdef GH_ESP_BUILD

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "core/bridge.h"

namespace gh {

class BridgeTCP : public Bridge {
   public:
    BridgeTCP(GyverHub* hub, IPAddress ip = INADDR_NONE, uint16_t port = 80) : Bridge(hub, Connection::HTTP, GyverHub::parseHook), ip(ip), port(port) {
        setMode(0, 1, 0);
    }

    // установить порт удалённого устройства
    void setPort(uint16_t port) {
        this->port = port;
    }

    // установить IP удалённого устройства
    void setIP(const IPAddress& ip) {
        this->ip = ip;
    }

    void send(BridgeData& data) {
        WiFiClient client;
        if (client.connect(ip, port)) {
            client.print("GET /hub/");
            client.print(data.text);
            client.print(" HTTP/1.1\r\nHost: ");
            client.print(ip);
            client.print("\r\nConnection: close\r\n\r\n");

            while (client.available()) client.read();
            // client.stop();
        }
    }

   private:
    IPAddress ip;
    uint16_t port;
};

}  // namespace gh

#endif  // GH_ESP_BUILD