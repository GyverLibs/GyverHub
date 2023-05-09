#pragma once
#include "config.h"

#ifdef GH_ESP_BUILD
#define GH_CONN_AMOUNT 4
#else
#define GH_CONN_AMOUNT 2
#endif

// причина перезагрузки
enum GHreason_t {
    GH_REB_NONE,
    GH_REB_BUTTON,
    GH_REB_OTA,
    GH_REB_OTA_URL,
};

// тип подключения
enum GHconn_t {
    GH_MANUAL,
    GH_STREAM,
    GH_WS,
    GH_MQTT,
    GH_HTTP,
    GH_SYSTEM,
};

// статус системы
enum GHstate_t {
    GH_IDLE,
    GH_START,
    GH_STOP,

    GH_CONNECTING,
    GH_CONNECTED,
    GH_DISCONNECTED,
    GH_ERROR,

    GH_UNKNOWN,
    GH_DISCOVER_ALL,
    GH_DISCOVER,
    GH_FOCUS,
    GH_UNFOCUS,

    GH_SET,
    GH_CLICK,
    GH_CLI,
    GH_PING,

    GH_READ_HOOK,
    GH_SET_HOOK,
    GH_INFO,
    GH_REBOOT,
    GH_FSBR,
    GH_DELETE,
    GH_RENAME,

    GH_DOWNLOAD,
    GH_DOWNLOAD_CHUNK,
    GH_DOWNLOAD_ERROR,
    GH_DOWNLOAD_ABORTED,
    GH_DOWNLOAD_FINISH,

    GH_UPLOAD,
    GH_UPLOAD_CHUNK,
    GH_UPLOAD_ERROR,
    GH_UPLOAD_ABORTED,
    GH_UPLOAD_FINISH,

    GH_OTA,
    GH_OTA_CHUNK,
    GH_OTA_ERROR,
    GH_OTA_ABORTED,
    GH_OTA_FINISH,

    GH_OTA_URL,
};

struct GHstatus {
    GHconn_t conn;
    GHstate_t state;
};