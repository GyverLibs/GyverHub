#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "types.h"

namespace ghc {

gh::CMD __attribute__((weak)) getCMD(const char* str) {
    switch (sutil::hash(str)) {
        case sutil::SH("ui"):              return gh::CMD::UI;
        case sutil::SH("ping"):            return gh::CMD::Ping;
        case sutil::SH("unfocus"):         return gh::CMD::Unfocus;
        case sutil::SH("info"):            return gh::CMD::Info;
        case sutil::SH("files"):           return gh::CMD::Files;
        case sutil::SH("format"):          return gh::CMD::Format;
        case sutil::SH("reboot"):          return gh::CMD::Reboot;
        case sutil::SH("fetch_next"):      return gh::CMD::FetchNext;
        case sutil::SH("data"):            return gh::CMD::Data;
        case sutil::SH("set"):             return gh::CMD::Set;
        case sutil::SH("get"):             return gh::CMD::Get;
        case sutil::SH("read"):            return gh::CMD::Read;
        case sutil::SH("cli"):             return gh::CMD::CLI;
        case sutil::SH("delete"):          return gh::CMD::Delete;
        case sutil::SH("rename"):          return gh::CMD::Rename;
        case sutil::SH("mkfile"):          return gh::CMD::Create;
        case sutil::SH("fs_abort"):        return gh::CMD::FsAbort;
        case sutil::SH("fetch"):           return gh::CMD::Fetch;
        case sutil::SH("upload"):          return gh::CMD::Upload;
        case sutil::SH("upload_chunk"):    return gh::CMD::UploadChunk;
        case sutil::SH("ota"):             return gh::CMD::Ota;
        case sutil::SH("ota_chunk"):       return gh::CMD::OtaChunk;
        case sutil::SH("ota_url"):         return gh::CMD::OtaUrl;
        case sutil::SH("unix"):            return gh::CMD::Unix;
        default:                           return gh::CMD::Unknown;
    }
}

}