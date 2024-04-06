#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "types.h"
#include "hub_macro.hpp"

namespace ghc {

gh::CMD __attribute__((weak)) getCMD(GHTREF cmd) {
    switch (cmd.hash()) {
        case su::SH("ui"):              return gh::CMD::UI;
        case su::SH("ping"):            return gh::CMD::Ping;
        case su::SH("unfocus"):         return gh::CMD::Unfocus;
        case su::SH("info"):            return gh::CMD::Info;
        case su::SH("files"):           return gh::CMD::Files;
        case su::SH("format"):          return gh::CMD::Format;
        case su::SH("reboot"):          return gh::CMD::Reboot;
        case su::SH("fetch_next"):      return gh::CMD::FetchNext;
        case su::SH("data"):            return gh::CMD::Data;
        case su::SH("set"):             return gh::CMD::Set;
        case su::SH("get"):             return gh::CMD::Get;
        case su::SH("read"):            return gh::CMD::Read;
        case su::SH("cli"):             return gh::CMD::CLI;
        case su::SH("delete"):          return gh::CMD::Delete;
        case su::SH("rename"):          return gh::CMD::Rename;
        case su::SH("mkfile"):          return gh::CMD::Create;
        case su::SH("fs_abort"):        return gh::CMD::FsAbort;
        case su::SH("fetch"):           return gh::CMD::Fetch;
        case su::SH("upload"):          return gh::CMD::Upload;
        case su::SH("upload_chunk"):    return gh::CMD::UploadChunk;
        case su::SH("ota"):             return gh::CMD::Ota;
        case su::SH("ota_chunk"):       return gh::CMD::OtaChunk;
        case su::SH("ota_url"):         return gh::CMD::OtaUrl;
        case su::SH("unix"):            return gh::CMD::Unix;
        case su::SH("location"):        return gh::CMD::Location;
        default:                        return gh::CMD::Unknown;
    }
}

}