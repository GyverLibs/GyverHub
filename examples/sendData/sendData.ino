// пример с общением между устройствами
#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

// можно добавить мосты UDP и TCP
#include <bridges/tcp.h>
#include <bridges/udp.h>
gh::BridgeTCP tcp(&hub);
gh::BridgeUDP udp(&hub, WIFI_STA);

void build(gh::Builder& b) {
}

void setup() {
    Serial.begin(115200);
    Serial.println();

#ifdef GH_ESP_BUILD
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    hub.mqtt.config("test.mosquitto.org", 1883);
#endif

    // мост udp работает на отправку и приём, его можно добавить в хаб
    hub.addBridge(&udp);
    // udp.setIP(IPAddress(192, 168, 1, 194));  // отправлять на конкретный ip
    // udp.setBroadcast();  // отправлять всем

    // мост TCP работает только на отправку (по сути принимает он на встроенный HTTP)
    tcp.setIP(IPAddress(192, 168, 1, 194));

    hub.config(F("MyDevices"), F("ESP"), F(""));
    hub.onBuild(build);
    hub.begin();

    // принимаем данные
    hub.onData([](gh::Data data) {
        Serial.print(gh::readConnection(data.client.connection));
        Serial.print(", data: ");
        Serial.print(data.name);
        Serial.print('=');
        Serial.println(data.value);
    });
}

void loop() {
    hub.tick();

    static gh::Timer tmr(2000);
    if (tmr) {
        // отправляем всем
        // hub.sendDataAll(&udp, "udp", "value");
        hub.sendDataAll(&tcp, "tcp", "value");
        // hub.sendDataAll(&hub.mqtt, "mqtt", "value");
    }
}