// пример работы через MQTT через PubSubClient
// работа с PubSubClient взята из примера mqtt_esp8266.ino

#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

// отключаем встроенную реализацию MQTT (для esp)
#define GH_NO_MQTT

#include <GyverHub.h>
GyverHub hub;

// MQTT
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class HubMQTT : public gh::Bridge {
   public:
    HubMQTT(void* hub) : gh::Bridge(hub, gh::Connection::MQTT, GyverHub::parseHook), mqtt(espClient) {}

    void begin() {
        mqtt.setServer("test.mosquitto.org", 1883);
        mqtt.setCallback([this](char* topic, uint8_t* data, uint16_t len) {
            uint16_t tlen = strlen(topic);
            char topic_buf[tlen + 1];
            memcpy(topic_buf, topic, tlen);
            topic_buf[tlen] = 0;

            char data_buf[len + 1];
            memcpy(data_buf, data, len);
            data_buf[len] = 0;

            parse(sutil::AnyText(topic_buf, tlen), sutil::AnyText(data_buf, len));
        });
    }
    void end() {
        mqtt.disconnect();
    }
    void tick() {
        if (!mqtt.connected()) reconnect();
        mqtt.loop();
    }
    void send(gh::BridgeData& data) {
        if (!mqtt.connected()) return;
        mqtt.beginPublish(data.topic.c_str(), data.text.length(), 0);
        mqtt.print(data.text);
        mqtt.endPublish();
    }

   private:
    WiFiClient espClient;
    PubSubClient mqtt;

    void reconnect() {
        while (!mqtt.connected()) {
            String clientId = "hub-";
            clientId += String(random(0xffff), HEX);
            if (mqtt.connect(clientId.c_str())) {
                Serial.println("connected");
                mqtt.subscribe(hub.topicDiscover().c_str());
                mqtt.subscribe(hub.topicHub().c_str());
            } else {
                delay(1000);
            }
        }
    }
};

HubMQTT mqtt(&hub);

// билдер
void build(gh::Builder& b) {
    static int val;
    b.Title(F("Hello!"));
    b.Slider(&val);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(AP_SSID, AP_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    hub.config(F("MyDevices"), F("ESP"), F(""));
    hub.onBuild(build);
    hub.addBridge(&mqtt);
    hub.begin();
}

void loop() {
    hub.tick();
}