#define AP_SSID "Alex"
#define AP_PASS ""

#include <GyverHUB.h>
//https://fontawesome.com/v5/cheatsheet/free/solid
GyverHUB device("MyDevices", "GYVER", "");
//GyverHUB device("MyDevices", "TCP", "");
//GyverHUB device("MyDevices", "ESP32", "");

bool sw;
int sld = 20;
GPdate gdate(2023, 3, 10);
GPtime gtime(12, 30);
int sel;
char inp[20] = "my text";
String pass;
int tab = 0;

void build_app() {
  device.addUpdate("lbl,sw,myled");

  device.addStatus("some status");
  device.addTabs("tabs", "Tab 1,MY TAB,tab 2,tab 3,TAB 4", tab);
  device.addTitle("Header");
  device.addLabel("lbl", "Some label", "label red", GP_BLUE);
  device.addLED("myled", "Status", GP_RED);

  device.addButtons("b1,b2,b3", "Button 1,MY BUTTON,Start");
  device.addButtonsIcons("play,stop,pause", ",,");
  device.addButtonIcon("b5", "");

  device.addTitle("Inputs");
  device.addInput("inp", "Text input", inp);
  device.addPass("pass", "Pass input", pass);
  device.addSlider("sld", "Slider", sld, 0, 100);
  device.addSwitch("sw", "My switch", sw);
  device.addSelect("sel", "List picker", "kek,puk,lol ass", sel);

  device.addTitle("Date Time");
  device.addDate("date", "Date select", gdate);
  device.addTime("time", "Time select", gtime);
}

bool holdFlag2;  // флаг удержания второй кнопки
void action_app() {
  if (device.click()) {
    Serial.println(device.clickName() + ':' + device.getString());
    if (device.clickInt("tabs", tab)) device.refresh();
    if (device.click("b1")) Serial.println("click b1");
    device.clickStr("inp", inp);
    device.clickInt("sld", sld);
    if (device.click("sw")) sw = device.getBool();
    device.clickDate("date", gdate);
    device.clickTime("time", gtime);
    device.clickInt("sel", sel);
    device.clickString("pass", pass);
  }

  if (device.hold()) {
    // обработка отдельно нажатия и отпускания
    if (device.clickDown("play")) Serial.println("Press play");
    if (device.clickUp("play")) Serial.println("Release play");

    holdFlag2 = device.hold("pause");
  }

  if (device.update()) {
    device.updateInt("lbl", random(50));
    device.updateBool("sw", random(2));
    static bool v;
    v = !v;
    device.updateColor("myled", v ? GPcolor(GP_GREEN) : GPcolor(GP_RED));
  }
}

void action() {
}

void setup() {
  startup();
  device.setupMQTT("broker.mqttdashboard.com", 1883);
  device.begin();

  device.attachBuild(build_app);
  device.attach(action_app);
}

void loop() {
  if (device.statusChanged()) Serial.println(device.status());
  device.tick();

  // проверяем через свой флаг
  if (holdFlag2) asyncPrint("hold pause");

  // в принципе можно и так, это будет нагружать процессор сравнением строк
  // только пока кнопка удерживается
  if (device.hold("stop")) asyncPrint("hold stop");
}

void asyncPrint(const char* str) {
  static uint32_t tmr;
  if (millis() - tmr >= 500) {
    tmr = millis();
    Serial.println(str);
  }
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
  /*
    WiFi.mode(WIFI_AP);
      WiFi.softAP("My HUB");*/
}
