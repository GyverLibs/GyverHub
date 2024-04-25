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
#include <WiFiUdp.h>

#include "core/bridge.h"

namespace gh {

class BridgeUDP : public Bridge {
   public:
    BridgeUDP(GyverHub* hub, WiFiMode_t wmode, IPAddress ip = INADDR_NONE, uint16_t port = 1234) : Bridge(hub, Connection::UDP, GyverHub::parseHook), wmode(wmode), ip(ip) {
        setPort(port);
    }

    using Bridge::setPort;

    // установить IP удалённого устройства
    void setIP(const IPAddress& ip) {
        this->ip = ip;
    }

    // установить широковещательный IP
    void setBroadcast() {
        if (wmode == WIFI_STA) {
            ip = broadcastIP(WiFi.localIP(), WiFi.subnetMask());
        } else {
#ifdef ESP8266
            struct ip_info info;
            wifi_get_ip_info(SOFTAP_IF, &info);
            ip = broadcastIP(info.ip.addr, info.netmask.addr);
#else
            ip = WiFi.softAPBroadcastIP();
#endif
        }
    }

    void begin() {
        udp.begin(getPort());
    }

    void end() {
        udp.stop();
    }

    void tick() {
        int packetSize = udp.parsePacket();
        if (packetSize) {
            char* buf = new char[packetSize];
            if (!buf) return;
            int n = udp.read(buf, packetSize);
            parse(su::Text(buf, n));
            delete[] buf;
        }
    }

    void send(BridgeData& data) {
        if (isAnswer()) {
            udp.beginPacket(udp.remoteIP(), udp.remotePort());
        } else {
            if (ip == INADDR_NONE) setBroadcast();
            udp.beginPacket(ip, getPort());
        }
        udp.print(data.text);
        udp.endPacket();
    }

   private:
    WiFiUDP udp;
    WiFiMode_t wmode;
    IPAddress ip;

    IPAddress broadcastIP(const IPAddress& ip, const IPAddress& subnet) {
        IPAddress broadcast;
        for (int i = 0; i < 4; i++) {
            broadcast[i] = ip[i] | (~subnet[i]);
        }
        return broadcast;
    }
};

}  // namespace gh

#endif  // GH_ESP_BUILD