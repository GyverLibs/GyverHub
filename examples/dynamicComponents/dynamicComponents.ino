#define AP_SSID "Alex"
#define AP_PASS ""

#include <GyverHUB.h>
GyverHUB device("MyDevices", "Dynamic Tabs", "");

#define MAX_TEXT 20
#define DYN_AM 5
bool sws[DYN_AM];
int slds[DYN_AM];
char inputs[DYN_AM][20];
int tab = 0;

void build_app() {
  device.addTabs("tab", "Sliders,Switches,Inputs", tab);
  // выбор вкладки
  switch (tab) {
    case 0: // создём слайдеры с именами sld/0, sld/1 ...
      for (int i = 0; i < DYN_AM; i++) {
        device.addSlider(String("sld/") + i, String("Slider #") + i, slds[i], 0, 100);
      }
      break;

    case 1: // создём кнопки с именами sw/0, sw/1 ..
      for (int i = 0; i < DYN_AM; i++) {
        device.addSwitch(String("sw/") + i, String("Switch #") + i, sws[i]);
      }
      break;

    case 2: // создём инпуты с именами inp/0, inp/1 ...
      for (int i = 0; i < DYN_AM; i++) {
        device.addInput(String("inp/") + i, String("Input #") + i, inputs[i]);
      }
      break;
  }
}

void action_app() {
  if (device.clickSub("sld")) {                 // начинается с sld
    int num = device.clickNameSub(1).toInt();   // получаем номер
    int val = device.getInt();                  // получаем значение
    Serial.println(String("Slider #") + num + ": " + val);
    slds[num] = val;
  }

  if (device.clickSub("sw")) {                  // начинается с sw
    int num = device.clickNameSub(1).toInt();   // получаем номер
    int val = device.getInt();                  // получаем значение
    Serial.println(String("Change sw #") + num + " to " + val);
    sws[num] = val;
  }

  if (device.clickSub("inp")) {                 // начинается с inp
    int num = device.clickNameSub(1).toInt();   // получаем номер
    device.copyStr(inputs[num], MAX_TEXT);      // получаем значение
    Serial.println(String("Input #") + num + " to " + inputs[num]);
  }
  
  if (device.clickInt("tab", tab)) device.refresh();
}

void setup() {
  startup();
  device.setupMQTT("broker.mqttdashboard.com", 1883);
  device.begin();

  device.attachBuild(build_app);
  device.attach(action_app);
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
