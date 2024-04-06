// пример работы через Serial полностью вручную

#include <Arduino.h>

// можно выключить остальную связь
#define GH_NO_HTTP
#define GH_NO_WS
#define GH_NO_MQTT

#include <GyverHub.h>
GyverHub hub;

class HubSerial : public gh::Bridge {
   public:
    HubSerial(void* hub, gh::Connection conn) : gh::Bridge(hub, conn, GyverHub::parseHook) {}

    void tick() {
        if (Serial.available()) {
            String str = Serial.readStringUntil('\0');
            parse(str);
        }
    }
    void send(gh::BridgeData& data) {
        Serial.print(data.text);
    }
};

HubSerial serial(&hub, gh::Connection::Serial);

// билдер
void build(gh::Builder& b) {
    static int val;
    b.Title(F("Hello!"));
    b.Slider(&val);
}

void setup() {
    Serial.begin(115200);

    hub.config(F("MyDevices"), F("AVR"), F(""));
    hub.onBuild(build);
    hub.addBridge(&serial);
    hub.begin();
}

void loop() {
    hub.tick();
}