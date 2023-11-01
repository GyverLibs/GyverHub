// ESP32-CAM DEMO

// WiFi
#define AP_SSID ""
#define AP_PASS ""
#include <Arduino.h>
#include <GyverHub.h>
GyverHub hub("MyDevices", "ESP32-CAM", "");

#include "camera.h"  // идёт в папке со скетчем
camera_fb_t* frame;

void build() {
  hub.BeginWidgets();
  hub.Image_("cam", "frame.jpg");
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

  cam_init(FRAMESIZE_QVGA);

  hub.onFetch([](String & path, bool start) {
    if (path == "frame.jpg") {
      if (start) {
        frame = esp_camera_fb_get();
        if (frame) hub.fetchBytes((byte*)frame->buf, frame->len);
      } else {
        esp_camera_fb_return(frame);

        // сразу начать скачивание следующего кадра
        // hub.sendUpdate("cam", "");
      }
    }
  });
}

void loop() {
  hub.tick();
}
