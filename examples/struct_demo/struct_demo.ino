// DATA STRUCT

// WiFi
#define AP_SSID ""
#define AP_PASS ""

#include <Arduino.h>
#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");

struct Data {
    bool sw;
    char inp[10];
    uint16_t sld;
    float spin;
    Stamp date;
    GHcolor col;
    GHflags flags;
};

Data data;

void build() {
    // Задача - получить сигнал о том, что было изменение и
    // нужно сохранить данные в память (EEPROM, файл итд)

    // ================= СПОСОБ 1 =================
    GHbuild b = hub.getBuild();

    hub.Switch(F("sw"), &data.sw, 0);
    hub.Input(F("inp_c"), &data.inp, GH_CSTR, 0, 10);
    hub.Slider(F("sld1"), &data.sld, GH_INT16);
    hub.Spinner(F("spin"), &data.spin, GH_FLOAT);
    hub.Date(F("date"), &data.date);
    hub.Color(F("color"), &data.col);
    hub.Flags(F("flags"), &data.flags);

    // было действие установки (ЛЮБОЕ НА СТРАНИЦЕ)
    if (b.action.type == GH_ACTION_SET) {
        // сохраняем структуру data
    }

    // ================= СПОСОБ 2 =================
    bool flag = 0;
    flag |= hub.Switch(F("sw"), &data.sw, 0);
    flag |= hub.Input(F("inp_c"), &data.inp, GH_CSTR, 0, 10);
    flag |= hub.Slider(F("sld1"), &data.sld, GH_INT16);
    flag |= hub.Spinner(F("spin"), &data.spin, GH_FLOAT);
    flag |= hub.Date(F("date"), &data.date);
    flag |= hub.Color(F("color"), &data.col);
    flag |= hub.Flags(F("flags"), &data.flags);

    // если хоть один из компонентов кликнут, flag будет 1
    // в отличие от первого способа, можно гибко разбивать компоненты
    // на группы, вводя несколько флагов для разных структур
    if (flag) {
        // сохраняем структуру data
    }
}

void setup() {
    Serial.begin(115200);

#ifdef GH_ESP_BUILD
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    // hub.setupMQTT("test.mosquitto.org", 1883);
#endif

    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();
}