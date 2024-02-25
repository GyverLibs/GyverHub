#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

#include <GyverHub.h>
GyverHub hub;

// билдер
void build(gh::Builder& b) {
    b.Title("My Dashboard");
    {
        gh::Row r(b);
        b.Input().value("input text");
        b.Pass().value("pass");
    }
    {
        gh::Row r(b);
        b.InputArea().value("big \"text\\\"\narea");
    }
    {
        gh::Row r(b);
        b.Slider().value(10);
        b.Spinner().value(3.14);
    }
    {
        gh::Row r(b);
        b.Date();
        b.Time();
    }

    b.DateTime();
    gh::Flags f;
    f.set(3);
    b.Flags(&f).text("select;some;flag;monday;friday");

    {
        gh::Row r(b);
        b.Select().text("first;second;third");
        b.Tabs().text("tab 1;tab 2;tab 3");
    }
    {
        gh::Row r(b);
        b.Button();
        b.LED().value(1);
        b.Switch();
        b.SwitchIcon().icon("");
    }
    {
        gh::Row r(b);
        b.Label("some label");
        b.Display("Some\ntext");
    }
    {
        gh::Row r(b);
        b.Joystick();
        b.Dpad();
    }
    {
        gh::Row r(b);
        b.GaugeRound().value(33).unit("%");
        {
            gh::Col c(b);
            b.Gauge().value(63);
            b.GaugeLinear().value(12);
        }
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

    hub.mqtt.config("test.mosquitto.org", 1883);  // + MQTT

    // ИЛИ

    // режим точки доступа
    // WiFi.mode(WIFI_AP);
    // WiFi.softAP("My Hub");
    // Serial.println(WiFi.softAPIP());
#endif

    hub.config(F("MyDevices"), F("ESP"));
    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();
}