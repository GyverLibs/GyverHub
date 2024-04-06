// пример работы через Websocket + HTTP с ESP8266WebServer
#include <Arduino.h>

#define AP_SSID "Alex"
#define AP_PASS "alexpass"

// отключаем встроенную реализацию HTTP+WS
#define GH_NO_HTTP
#define GH_NO_WS
// #define GH_NO_MQTT

// указываем что трансфер файлов будет по вебсокет,
// потому что работу с файлами силами сервера мы не написали
#define GH_NO_HTTP_TRANSFER

#include <GyverHub.h>
GyverHub hub;

// ======================= WS =======================
#include <WebSocketsServer.h>

class HubWS : public gh::Bridge {
   public:
    HubWS(void* hub) : gh::Bridge(hub, gh::Connection::WS, GyverHub::parseHook), ws(GH_WS_PORT, "", "hub") {}

    void begin() {
        ws.onEvent([this](uint8_t num, WStype_t type, uint8_t* data, size_t len) {
            switch (type) {
                case WStype_CONNECTED:
                    setFocus();
                    break;

                case WStype_DISCONNECTED:
                    clearFocus();
                    break;

                case WStype_TEXT:
                    clientID = num;
                    parse(su::Text((char*)data, len));
                    break;

                default:
                    break;
            }
        });

        ws.begin();
    }
    void end() {
        ws.close();
    }
    void tick() {
        ws.loop();
    }
    void send(gh::BridgeData& data) {
        if (data.broadcast) {
            ws.broadcastTXT(data.text.str(), data.text.length());
        } else {
            ws.sendTXT(clientID, data.text.str(), data.text.length());
        }
    }

   private:
    WebSocketsServer ws;
    uint8_t clientID = 0;
};

HubWS ws(&hub);

// ======================= HTTP =======================

#ifdef ESP8266
#include <ESP8266WebServer.h>
#define SERVER_T ESP8266WebServer
#else
#include <WebServer.h>
#define SERVER_T WebServer
#endif
#include <uri/UriGlob.h>

// делаем класс на основе Bridge
class HubHTTP : public gh::Bridge {
   public:
    HubHTTP(void* hub) : gh::Bridge(hub, gh::Connection::HTTP, GyverHub::parseHook), server(GH_HTTP_PORT) {}

    void begin() {
        server.on(UriGlob("/hub/*"), HTTP_GET, [this]() {
            server.setContentLength(CONTENT_LENGTH_UNKNOWN);
            server.send(200, "text/plain");
            String url = server.uri().substring(5);  // 5 == /hub/
            parse(url);
            server.sendContent("");
            server.client().stop();
        });

        server.begin();
        server.enableCORS(true);  // нужно для работы с приложением и внешним сайтом
    }
    void end() {
        server.stop();
    }
    void tick() {
        server.handleClient();
    }
    void send(gh::BridgeData& data) {
        if (!isAnswer()) return;    // не можем отправлять, только отвечать
        server.sendContent(data.text.str(), data.text.length());
    }

    SERVER_T server;

   private:
};

HubHTTP http(&hub);

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
    hub.addBridge(&http);
    hub.addBridge(&ws);
    hub.begin();
}

void loop() {
    hub.tick();
}