#pragma once

#define GH_LIB_VERSION "v0.1b"  // версия библиотеки
#define GH_HTTP_PATH "/fs/"     // путь к папке с файлами с HTTP доступом
#define GH_CONN_TOUT 5          // таймаут соединения, с
#define GH_HTTP_PORT 80         // http порт
#define GH_WS_PORT 81           // websocket порт
#define GH_DOWN_CHUNK_SIZE 512  // размер чанка при скачивании с платы
#define GH_UPL_CHUNK_SIZE 200   // размер чанка при загрузке на плату
#define GH_FS_DEPTH 5           // глубина сканирования файловой системы (esp32)
#define GH_FS LittleFS          // файловая система
#define GH_MQTT_RECONNECT 5000  // период переподключения MQTT
#define GH_CACHE_PRD "max-age=604800"   // период кеширования файлов для портала

#if (defined(ESP8266) || defined(ESP32))
#define GH_ESP_BUILD
#endif