#pragma once
#include <Arduino.h>

#include "hub_macro.hpp"
#include "types.h"

namespace gh {

enum Module : uint16_t {
    ModUI = (1ul << 0),
    ModInfo = (1ul << 1),
    ModSet = (1ul << 2),
    ModRead = (1ul << 3),
    ModGet = (1ul << 4),
    ModData = (1ul << 5),
    ModReboot = (1ul << 6),
    ModFiles = (1ul << 7),
    ModFormat = (1ul << 8),
    ModDelete = (1ul << 9),
    ModRename = (1ul << 10),
    ModCreate = (1ul << 11),
    ModFetch = (1ul << 12),
    ModUpload = (1ul << 13),
    ModOta = (1ul << 14),
    ModOtaUrl = (1ul << 15),
};

}  // namespace gh

namespace ghc {

struct Modules {
    uint16_t mods = 0;

    void set(uint16_t nmods) {
        mods &= ~nmods;
    }
    void clear(uint16_t nmods) {
        mods |= nmods;
    }

    void setAll() {
        mods = 0;
    }
    void clearAll() {
        mods = UINT16_MAX;
    }
    bool read(gh::Module m) {
        return !(mods & (uint16_t)m);
    }
    bool read(uint16_t m) {
        return !(mods & (uint16_t)m);
    }

    bool checkCMD(gh::CMD cmd) {
        switch (cmd) {
            case gh::CMD::UI:
                return read(gh::Module::ModUI);

            case gh::CMD::Info:
                return read(gh::Module::ModInfo);

            case gh::CMD::Data:
                return read(gh::Module::ModData);

            case gh::CMD::Set:
                return read(gh::Module::ModSet);

#ifdef GH_ESP_BUILD
            case gh::CMD::Reboot:
                return read(gh::Module::ModReboot);

            case gh::CMD::Ota:
            case gh::CMD::OtaChunk:
                return read(gh::Module::ModOta);

            case gh::CMD::OtaUrl:
                return read(gh::Module::ModOtaUrl);

#ifndef GH_NO_FS
            case gh::CMD::FsAbort:
                return read(gh::Module::ModFetch) || read(gh::Module::ModUpload) || read(gh::Module::ModOta);
            case gh::CMD::Files:
                return read(gh::Module::ModFiles);

            case gh::CMD::Format:
                return read(gh::Module::ModFormat);

            case gh::CMD::Delete:
                return read(gh::Module::ModDelete);

            case gh::CMD::Rename:
                return read(gh::Module::ModRename);

            case gh::CMD::Create:
                return read(gh::Module::ModCreate);
#ifndef GH_NO_FETCH
            case gh::CMD::Fetch:
            case gh::CMD::FetchNext:
                return read(gh::Module::ModFetch);
#endif
#ifndef GH_NO_UPLOAD
            case gh::CMD::Upload:
            case gh::CMD::UploadChunk:
                return read(gh::Module::ModUpload);
#endif
#else
            case gh::CMD::Files:
            case gh::CMD::FsAbort:
            case gh::CMD::Format:
            case gh::CMD::Delete:
            case gh::CMD::Rename:
            case gh::CMD::Create:
            case gh::CMD::Fetch:
            case gh::CMD::FetchNext:
            case gh::CMD::Upload:
            case gh::CMD::UploadChunk:
                return 0;
#endif  // GH_NO_FS
#endif  // GH_ESP_BUILD
            default:
                return 1;
        }
    }
};
}  // namespace ghc