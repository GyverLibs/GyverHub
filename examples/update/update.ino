// UPDATE DEMO

// WiFi
#define AP_SSID ""
#define AP_PASS ""

#include <Arduino.h>
#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");

void build() {
    hub.BeginWidgets();
    hub.WidgetSize(50);

    // обновляемым компонентам нужно присвоить уникальные имена
    // чтобы было на кого отправлять обновления
    hub.Label_(F("lbl"));

    hub.WidgetSize(25);
    hub.LED_(F("led"));

    // по клику по кнопке
    if (hub.ButtonIcon(0, F(""))) {
        static bool led;
        led = !led;
        hub.sendUpdate("ga", String(random(100)));  // обновляем шкалу
        hub.sendUpdate("led", String(led));         // обновляем светодиод
    }

    hub.WidgetSize(50);
    hub.Canvas_(F("cv"));
    hub.Gauge_(F("ga"));
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

    hub.setupMQTT("test.mosquitto.org", 1883);
#endif

    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();

    // таймер на 500 мс
    // добавим случайных кругов на холст
    static GHtimer tmr1(500);
    if (tmr1.ready()) {
        GHcanvas cv;
        cv.fill(GHcolor(random(255), random(255), random(255)), random(100, 255));
        cv.circle(random(0, 40) * 10, random(0, 30) * 10, random(5, 30));
        hub.sendCanvas(F("cv"), cv);
    }

    // таймер на 1с
    // выведем миллис на label
    static GHtimer tmr2(1000);
    if (tmr2.ready()) {
        hub.sendUpdate("lbl", String(millis()));
    }
}