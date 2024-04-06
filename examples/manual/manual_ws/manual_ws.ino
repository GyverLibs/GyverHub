// пример работы через Websocket + HTTP вручную с WiFiServer

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
#include <ESP8266WiFi.h>

// делаем класс на основе Bridge
class HubHTTP : public gh::Bridge {
   public:
    HubHTTP(void* hub) : gh::Bridge(hub, gh::Connection::HTTP, GyverHub::parseHook), server(GH_HTTP_PORT) {}

    void begin() {
        server.begin();
    }
    void end() {
        server.stop();
    }
    void tick() {
        WiFiClient client = server.accept();
        if (client) {
            String req = client.readStringUntil('/');  // "GET /"
            req = client.readStringUntil('\r');        // первая строка до конца
            while (client.available()) client.read();  // пропускаем остальное

            // нас интересует url /hub/... HTTP/, берём нужный сегмент
            int start = req.indexOf(F("hub/"));
            int end = req.indexOf(F(" HTTP/"));
            if (start < 0 || end < 0) return;
            req = req.substring(start + 4, end);

            // отвечаем клиенту с no-cors хэдером (для работы с внешним сайтом или приложением)
            client.print(F(
                "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n"
                "Access-Control-Allow-Origin:*\r\n"
                "Access-Control-Allow-Private-Network: true\r\n"
                "Access-Control-Allow-Methods:*\r\n\r\n"));

            // запоминаем клиента, тк ответ ему будет внутри send
            client_p = &client;
            parse(req);
            client_p = nullptr;
        }
    }
    void send(gh::BridgeData& data) {
        if (client_p) client_p->print(data.text);
    }

    WiFiServer server;

   private:
    // будем хранить указатель на клиента для ответов
    WiFiClient* client_p = nullptr;
};

HubHTTP http(&hub);

// ======================= BUILD =======================

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