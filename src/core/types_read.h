#pragma once
#include <Arduino.h>

#include "hub_macro.hpp"
#include "types.h"

namespace gh {
// TODO new cmd
// получить текстовое значение типа CMD для вывода в порт
FSTR __attribute__((weak)) readCMD(CMD cmd) {
    switch (cmd) {
        case CMD::UI: return F("UI");
        case CMD::Ping: return F("Ping");
        case CMD::Unfocus: return F("Unfocus");
        case CMD::Info: return F("Info");
        case CMD::Files: return F("Files");
        case CMD::Format: return F("Format");
        case CMD::Reboot: return F("Reboot");
        case CMD::FetchNext: return F("FetchNext");
        case CMD::Data: return F("Data");
        case CMD::Set: return F("Set");
        case CMD::Get: return F("Get");
        case CMD::Read: return F("Read");
        case CMD::CLI: return F("CLI");
        case CMD::Delete: return F("Delete");
        case CMD::Rename: return F("Rename");
        case CMD::Create: return F("Create");
        case CMD::FsAbort: return F("FsAbort");
        case CMD::Fetch: return F("Fetch");
        case CMD::Upload: return F("Upload");
        case CMD::UploadChunk: return F("UploadChunk");
        case CMD::Ota: return F("Ota");
        case CMD::OtaChunk: return F("OtaChunk");
        case CMD::OtaUrl: return F("OtaUrl");
        case CMD::Unix: return F("Unix");
        case CMD::Search: return F("Search");
        case CMD::Discover: return F("Discover");
        case CMD::Location: return F("Location");
        default: return F("Unknown");
    }
}

// получить текстовое значение типа Connection для вывода в порт
FSTR __attribute__((weak)) readConnection(Connection connection) {
    switch (connection) {
        case Connection::Serial: return F("Serial");
        case Connection::Bluetooth: return F("Bluetooth");
        case Connection::WS: return F("WS");
        case Connection::MQTT: return F("MQTT");
        case Connection::HTTP: return F("HTTP");
        case Connection::Telegram: return F("Telegram");
        case Connection::HTTP_WS: return F("HTTP_WS");
        case Connection::UDP: return F("UDP");
        case Connection::System: return F("System");
        case Connection::Custom: return F("Custom");
        default: return F("Unknown");
    }
}

// получить текстовое значение типа Action для вывода в порт
FSTR __attribute__((weak)) readAction(Action action) {
    switch (action) {
        case Action::UI: return F("UI");
        case Action::Read: return F("Read");
        case Action::Set: return F("Set");
        case Action::Get: return F("Get");
        case Action::None: return F("None");
        default: return F("Unknown");
    }
}

// получить текстовое значение типа Reboot для вывода в порт
FSTR __attribute__((weak)) readReboot(Reboot reason) {
    switch (reason) {
        case Reboot::None: return F("None");
        case Reboot::Button: return F("Button");
        case Reboot::Ota: return F("Ota");
        case Reboot::OtaUrl: return F("OtaUrl");
        default: return F("Unknown");
    }
}

}  // namespace gh