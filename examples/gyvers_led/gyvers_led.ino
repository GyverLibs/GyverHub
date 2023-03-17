#define AP_SSID "Alex"
#define AP_PASS ""

#include <GyverHUB.h>
GyverHUB device("MyDevices", "Gyver LED", "");

bool state;

void build_app() {
  device.addSwitch("led", "LED state", state);
}

void action_app() {
  if (device.clickBool("led", state)) {
    digitalWrite(LED_BUILTIN, !state);
  }
}

void setup() {
  startup();
  device.setupMQTT("broker.mqttdashboard.com", 1883);
  device.begin();

  device.attachBuild(build_app);
  device.attach(action_app);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);
}

void loop() {
  //if (device.statusChanged()) Serial.println(device.status());
  device.tick();
}

void startup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
}
