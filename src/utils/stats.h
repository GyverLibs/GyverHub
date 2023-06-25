#pragma once
#include "../config.hpp"

// тип info
enum GHinfo_t {
    GH_INFO_VERSION,
    GH_INFO_NETWORK,
    GH_INFO_MEMORY,
    GH_INFO_SYSTEM,
};

// причина перезагрузки
enum GHreason_t {
    GH_REB_NONE,
    GH_REB_BUTTON,
    GH_REB_OTA,
    GH_REB_OTA_URL,
};

// тип подключения
enum GHconn_t {
    GH_SERIAL,
    GH_BT,
    GH_WS,
    GH_MQTT,
    GH_SYSTEM,
};

#define GH_CONN_AMOUNT 4

// системные события
enum GHevent_t {
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
    GH_CLI,
    GH_PING,

    GH_READ_HOOK,
    GH_SET_HOOK,
    GH_INFO,
    GH_REBOOT,
    GH_FSBR,
    GH_FORMAT,
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