#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/client.h"
#include "core/types.h"
#include "hub_macro.hpp"
#include "transfer.h"
#include "ui/timer.h"

#ifdef ESP8266
#include <Updater.h>
#else
#include <Update.h>
#endif

namespace ghc {

class Updater : public ghc::TransferBase {
   public:
    Updater(gh::Client& client,
            gh::Reboot* reason,
            const char* id) : ghc::TransferBase(client, id, ghc::Tag::ota_err),
                              reason(reason) {}

    bool begin(GHTREF type) {
        bool ota_flash = 0;
        switch (type.hash()) {
            case sutil::SH("flash"):
                ota_flash = 1;
                break;
            case sutil::SH("fs"):
                ota_flash = 0;
                break;
            default:
                setError(gh::Error::WrongType);
                return 0;
        }
        tmr.setTime(GH_CONN_TOUT * 1000);
        tmr.startTimeout();

        size_t ota_size = 0;
        int ota_type = 0;

        if (ota_flash) {
            ota_type = U_FLASH;
#ifdef ESP8266
            ota_size = (size_t)((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
#else
            ota_size = UPDATE_SIZE_UNKNOWN;
#endif
        } else {
#ifdef ESP8266
            ota_type = U_FS;
            close_all_fs();
#ifndef GH_NO_FS
            ota_size = (size_t)&_FS_end - (size_t)&_FS_start;
#endif
#else
            ota_type = U_SPIFFS;
            ota_size = UPDATE_SIZE_UNKNOWN;
#endif
        }
        if (!::Update.begin(ota_size, ota_type)) {
            setError(gh::Error::Start);
        }
        if (hasError()) return 0;
        else answerCMD(Tag::ota_next);
        return 1;
    }

    void process(size_t typeHash, GHTREF data) {
        switch (typeHash) {
            case sutil::SH("next"):
                write64(data);
                if (!hasError()) answerCMD(Tag::ota_next);
                break;
            case sutil::SH("last"):
                lastChunk = 1;
                write64(data);
                end();
                if (!hasError()) answerCMD(Tag::ota_done);
                break;
            default:
                setError(gh::Error::WrongType);
                return;
        }
    }

    bool last() {
        return lastChunk;
    }

    bool write64(GHTREF data) {
        if (hasError()) return 0;
        uint16_t declen = data.sizeB64();
        uint8_t buf[declen];
        if (!data.decodeB64(buf, declen)) {
            setError(gh::Error::PacketDamage);
            return 0;
        }
        return write(buf, declen);
    }

    bool write(uint8_t* data, uint32_t len) {
        if (!hasError()) {
            tmr.restart();
            if (::Update.write(data, len) != len) setError(gh::Error::Write);
        }
        return !hasError();
    }

    bool end() {
        if (!hasError()) {
            if (::Update.end(true)) *reason = gh::Reboot::Ota;
            else setError(gh::Error::End);
        }
        return !hasError();
    }

    bool timeout() {
        if (tmr) setError(gh::Error::Timeout);
        return hasError();
    }

    void abort() {
        setError(gh::Error::Abort);
    }

    static void sendError(gh::Client& client, const char* id, gh::Error err) {
        ghc::TransferBase::sendError(client, id, err, ghc::Tag::ota_err);
    }

   private:
    gh::Reboot* reason;

    gh::Timer tmr;
    bool lastChunk = 0;
};

}  // namespace ghc