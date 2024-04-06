// пример работы через Serial при помощи hub stream

#include <Arduino.h>

// можно выключить остальную связь
#define GH_NO_HTTP
#define GH_NO_WS
#define GH_NO_MQTT

#include <GyverHub.h>
GyverHub hub;

#include <bridges/stream.h>
// подключить экземпляр любого Stream-класса
gh::BridgeStream stream(&hub, &Serial, gh::Connection::Serial);

// билдер
void build(gh::Builder& b) {
    static int val;
    b.Title(F("Hello!"));
    b.Slider(&val);
}

void setup() {
    Serial.begin(115200);
    
    // и подключить его в хаб
    hub.addBridge(&stream);

    hub.config(F("MyDevices"), F("AVR"), F(""));
    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();
}