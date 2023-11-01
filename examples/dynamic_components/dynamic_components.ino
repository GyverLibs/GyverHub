// динамически создаваемые компоненты

#include <Arduino.h>

#define AP_SSID ""
#define AP_PASS ""

#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP8266", "");

#define MAX_TEXT 20
#define DYN_MAX 5
bool sws[DYN_MAX];
int16_t slds[DYN_MAX];
char inputs[DYN_MAX][20];
uint8_t spin_am = 2;
uint8_t tab = 0;

void build() {
    hub.BeginWidgets();

    hub.WidgetSize(80);
    hub.Tabs(&tab, F("Sliders,Switches,Inputs,Buttons"));

    // спиннер с настройкой количества. По клику обновляем страницу
    hub.WidgetSize(20);
    if (hub.Spinner(&spin_am, GH_UINT8, F("Amount"), 0, DYN_MAX, 1)) {
        hub.refresh();
    }

    switch (tab) {
        case 0:
            hub.WidgetSize(100);
            for (int i = 0; i < spin_am; i++) {
                bool a = hub.Slider(&slds[i], GH_INT16, String("Slider #") + i);
                if (a) Serial.println(String("Set slider: #") + i + ", value: " + slds[i]);
            }
            break;
        case 1:
            hub.WidgetSize(25);
            for (int i = 0; i < spin_am; i++) {
                bool a = hub.Switch(&sws[i], String("Switch #") + i);
                if (a) Serial.println(String("Set switch: #") + i + ", value: " + sws[i]);
            }
            break;
        case 2:
            hub.WidgetSize(50);
            for (int i = 0; i < spin_am; i++) {
                bool a = hub.Input(&inputs[i], GH_CSTR, String("Input #") + i);
                if (a) Serial.println(String("Set input: #") + i + ", value: " + inputs[i]);
            }
            break;
        case 3:
            hub.WidgetSize(25);
            for (int i = 0; i < spin_am; i++) {
                // имена компонентов тоже можно генерировать, если это нужно
                bool a = (hub.Button_(String("btn/") + i, 0, String("Button #") + i) == 1);
                if (a) Serial.println(String("Pressed button: btn/") + i);
            }
            break;
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

    hub.setupMQTT("test.mosquitto.org", 1883);
#endif

    hub.onBuild(build);
    hub.begin();
}

void loop() {
    hub.tick();
}