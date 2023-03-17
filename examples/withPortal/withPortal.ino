#define AP_SSID "Alex"
#define AP_PASS ""

#include <GyverPortal.h>
GyverPortal ui;

#include <GyverHUB.h>
//https://fontawesome.com/v5/cheatsheet/free/solid
GyverHUB device("MyDevices", "My device", "");

void build_app() {
  device.addTitle("Some Title");
}
void action_app() {
  Serial.println(device.clickName() + ':' + device.getString());
}

void build() {
  GP.BUILD_BEGIN(GP_DARK);
  GP.UI_BEGIN("Portal", "/,/settings,/update", "Home,Settings,Update");
  GP.UI_END();
  GP.BUILD_END();
}
void action() {
}

void setup() {
  startup();
  //device.setupMQTT("broker.mqttdashboard.com", 1883);
  device.begin();

  device.attachBuild(build_app);
  device.attach(action_app);
  device.usePortal(true);

  ui.attach(action);
  ui.attachBuild(build);
  ui.start();
}

void loop() {
  //if (device.statusChanged()) Serial.println(device.status());
  
  ui.tick();
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
  /*WiFi.mode(WIFI_AP);
  WiFi.softAP("My Portal");*/
}
