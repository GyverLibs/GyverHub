#pragma once

namespace gh {

// ошибка
enum class Error : uint8_t {
    None,          // нет ошибки
    OpenFile,      // ошибка открытия файла
    FreeSpace,     // нет свободного места
    CrcMiss,       // ошибка контрольной суммы
    SizeMiss,      // не совпал размер
    Start,         // ошибка старта
    Write,         // ошибка записи
    End,           // ошибка завершения
    Abort,         // прервано
    Timeout,       // таймаут соединения
    Busy,          // загрузчик занят другим клиентом
    Memory,        // невозможно выделить память
    WrongClient,   // не тот клиент
    Forbidden,     // запрещено в request
    Disabled,      // модуль отключен
    WrongType,     // некорректный тип
    PacketDamage,  // пакет повреждён
    CantAlloc,     // ошибка выделения памяти
};

enum class Align : uint8_t {
    Left,
    Center,
    Right,
};

enum class Layer : uint8_t {
    OSM,
    GoogleStreet,
    GoogleSatellite,
    GoogleHybrid,
};

// тип данных
enum class Type : uint8_t {
    NULL_T,
    STR_T,     // String
    CSTR_T,    // char[]
    BOOL_T,    // bool
    INT8_T,    // int8_t
    UINT8_T,   // uint8_t
    INT16_T,   // int16_t
    UINT16_T,  // uint16_t
    INT32_T,   // int32_t
    UINT32_T,  // uint32_t
    INT64_T,   // int64_t
    UINT64_T,  // uint64_t
    FLOAT_T,   // float
    DOUBLE_T,  // double
    COLOR_T,   // gh::Colors
    FLAGS_T,   // gh::Flags
    PAIRS_T,   // Pairs
    PAIR_T,    // Pair
    LOG_T,     // gh::Log
    TEXT_T,    // Text
};

// тип билда
enum class Action : uint8_t {
    UI,
    Read,
    Set,
    Get,
    None,
};

// причина перезагрузки
enum class Reboot : uint8_t {
    None,
    Button,
    Ota,
    OtaUrl,
};

#define GH_CONN_AMOUNT 6

// тип подключения
enum class Connection : uint8_t {
    Serial,
    Bluetooth,
    WS,
    MQTT,
    HTTP,
    Telegram,
    UDP,
    Custom,
    System,
};

// системные события
enum class CMD : uint8_t {
    UI,
    Ping,
    Unfocus,
    Info,
    Files,
    Format,
    Reboot,
    FetchNext,
    Data,
    Set,
    Get,
    Read,
    CLI,
    Delete,
    Rename,
    Create,
    FsAbort,
    Fetch,
    Upload,
    UploadChunk,
    Ota,
    OtaChunk,
    OtaUrl,
    Unix,
    Search,
    Discover,
    Location,
    Unknown,
};

}  // namespace gh