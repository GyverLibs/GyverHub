#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/client.h"
#include "core/core_class.h"
#include "core/fs.h"
#include "core/packet.h"
#include "hub_macro.hpp"

#ifdef GH_ESP_BUILD
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#endif
#endif  // GH_ESP_BUILD

namespace gh {
class Info;
}

namespace ghc {
#ifdef GH_ESP_BUILD
typedef std::function<void(gh::Info& info)> InfoCallback;
#else
typedef void (*InfoCallback)(gh::Info& info);
#endif
}

namespace gh {
class Info {
    friend class ghc::HubCore;

   public:
    // тип info
    enum class Type : uint8_t {
        Version,
        Network,
        Memory,
        System,
    };

    Info(ghc::Packet& p, Type type, Client& client) : client(client), type(type), p(p) {}

    // добавить int/string/bool поле в info
    void add(GHTREF label, const sutil::AnyValue& val) {
        if (val.valid()) {
            if (val.type() == sutil::AnyText::Type::value) p.addInt(label, val);
            else p.addStringEsc(label, val);
        }
    }

    // добавить float поле в info
    void add(GHTREF label, double val, uint8_t dec) {
        p.addFloat(label, val, dec);
    }

    const Client& client;
    Type type;

   private:
    ghc::Packet& p;

    // =========================== STATIC ===========================

    static void _buildGroup(ghc::InfoCallback cb, ghc::Packet& p, Type type, Client& client) {
        if (cb) {
            Info info(p, type, client);
            cb(info);
        }
    }

    static void _build(ghc::InfoCallback cb, ghc::Packet& p, String& version, Client& client) {
        // ================ VERSION ================
        p.s += F("\"info\":{\"version\":{\"Library\":\"" GH_LIB_VERSION "\",");
        if (version.length()) p.addString(F("Firmware"), version);
        _buildGroup(cb, p, Type::Version, client);
        p.endObj();

        // ================ NETWORK ================
        p.s += (F("\"net\":{"));
#ifdef GH_ESP_BUILD
        switch (WiFi.getMode()) {
            case WIFI_AP:
                p.addString(F("Mode"), F("AP"));
                p.addString(F("AP_IP"), WiFi.softAPIP().toString());
                p.addString(F("AP_SSID"), WiFi.softAPSSID());
                p.addString(F("AP_MAC"), WiFi.softAPmacAddress());
                break;

            case WIFI_STA:
                p.addString(F("Mode"), F("STA"));
                p.addString(F("IP"), WiFi.localIP().toString());
                p.addString(F("SSID"), WiFi.SSID());
                p.addString(F("MAC"), WiFi.macAddress());
                break;

            case WIFI_AP_STA:
                p.addString(F("Mode"), F("AP+STA"));
                p.addString(F("AP_IP"), WiFi.softAPIP().toString());
                p.addString(F("AP_SSID"), WiFi.softAPSSID());
                p.addString(F("AP_MAC"), WiFi.softAPmacAddress());
                p.addString(F("IP"), WiFi.localIP().toString());
                p.addString(F("SSID"), WiFi.SSID());
                p.addString(F("MAC"), WiFi.macAddress());
                break;

            default:
                break;
        }
        p.addString(F("RSSI"), String(constrain(2 * (WiFi.RSSI() + 100), 0, 100)) + '%');
#endif
        _buildGroup(cb, p, Type::Network, client);
        p.endObj();

        // ================ MEMORY ================
        p.s += (F("\"memory\":{"));
#ifdef GH_ESP_BUILD
        p.beginArr(F("RAM"));
        p.addIntRaw(ESP.getFreeHeap());
        p.s += ",0],";
#ifndef GH_NO_FS
        p.beginArr(F("Flash"));
        p.addIntRaw(FS.usedSpace());
        p.comma();
        p.addIntRaw(FS.totalSpace());
        p.endArr();

        p.beginArr(F("Sketch"));
        p.addIntRaw(ESP.getSketchSize());
        p.comma();
        p.addIntRaw(ESP.getFreeSketchSpace());
        p.endArr();
#endif
#endif
        _buildGroup(cb, p, Type::Memory, client);
        p.endObj();

        // ================ SYSTEM ================
        p.s += (F("\"system\":{"));
        p.addInt(F("Uptime"), millis() / 1000ul);
        p.addString(F("Platform"), F(GH_PLATFORM));

#ifdef GH_ESP_BUILD
        p.addInt(F("CPU_MHz"), ESP.getCpuFreqMHz());
        p.addKey(F("Flash_chip"));
        p.quotes();
        p.addIntRaw(sutil::AnyValue(ESP.getFlashChipSize() / 1000.0, 1));
        p.s += " kB\",";
#endif
        _buildGroup(cb, p, Type::System, client);
        p.endObj();
        p.endObj();
    }
};

}  // namespace gh