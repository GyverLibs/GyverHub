#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

gh::Pos pos2;

// билдер
void build(gh::Builder& b) {
    if (b.beginRow()) {
        // первый обработаем в билдере
        gh::Pos pos1;
        if (b.Joystick(&pos1).click()) {
            Serial.print("joy 1: ");
            Serial.print(pos1.x);
            Serial.print(",");
            Serial.println(pos1.y);
        }

        // второй в loop
        // не возвращать в центр, значения по экспоненте
        b.Joystick(&pos2, 1, 1).color(gh::Colors::Red);

        b.endRow();
    }
}

void setup() {
    Serial.begin(115200);

#ifdef GH_ESP_BUILD
    // подключение к роутеру
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

    hub.config(F("MyDevices"), F("ESP"));
    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();

    if (pos2.changed()) {
        Serial.print("joy 2: ");
        Serial.print(pos2.x);
        Serial.print(",");
        Serial.println(pos2.y);
    }
}