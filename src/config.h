#pragma once

#define GH_VERSION "v1.0"       // версия библиотеки
#define GH_CONN_TOUT 5          // таймаут соединения, с
#define GH_HTTP_PORT 81         // http порт
#define GH_WS_PORT 82           // websocket порт
#define GH_DOWN_CHUNK_SIZE 512  // размер чанка при скачивании с платы
#define GH_UPL_CHUNK_SIZE 200   // размер чанка при загрузке на плату
#define GH_FS_DEPTH 5           // глубина сканирования файловой системы (esp32)
#define GH_FS LittleFS          // файловая система
#define GH_MQTT_RECONNECT 5000  // период переподключения MQTT

#if (defined(ESP8266) || defined(ESP32))
#define GH_ESP_BUILD
#endif