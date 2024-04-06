// пример работы через SoftSerial при помощи hub stream

#include <Arduino.h>

// можно выключить остальную связь
#define GH_NO_HTTP
#define GH_NO_WS
#define GH_NO_MQTT

#include <GyverHub.h>
GyverHub hub;

// SoftwareSerial на пинах 2 и 3, подключен Bluetooth
#include <SoftwareSerial.h>
SoftwareSerial bt(2, 3);

#include <bridges/stream.h>
// подключить экземпляр любого Stream-класса
gh::BridgeStream stream(&hub, &bt, gh::Connection::Bluetooth);

void build(gh::Builder& b) {
    static int val;
    b.Title(F("Hello!"));
    b.Slider(&val);
}

void setup() {
    bt.begin(9600);

    // и подключить его в хаб
    hub.addBridge(&stream);

    hub.config(F("MyDevices"), F("AVR"), F(""));
    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();
}