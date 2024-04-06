#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/client.h"
#include "core/fs.h"
#include "core/types.h"
#include "core/utils/crc32.h"
#include "hub_macro.hpp"
#include "transfer.h"
#include "ui/timer.h"

namespace ghc {

class Uploader : public ghc::TransferBase {
   public:
    Uploader(gh::Client& client,
             uint32_t id,
             bool safe = 0) : ghc::TransferBase(client, id, ghc::Tag::ota_err),
                              safe(safe) {}

    void setCRC(GHTREF crc) {
        upl_crc32 = crc.toInt32();
        crc32 = 0;
    }

    bool begin(GHTREF fpath, GHTREF sizetxt) {
        uint32_t size = sizetxt.toInt32();
        tmr.setTime(GH_CONN_TOUT * 1000);
        tmr.startTimeout();
        uint64_t space = gh::FS.freeSpace();
        if (!safe) {
            File f = gh::FS.openRead(fpath.toString());
            if (f) {
                space += f.size();  // overwrite
                f.close();
            }
        }
        if (size < space) {
            upl_size = size;
            fpath.toString(path);
            gh::FS.mkdir(path);
            if (safe) file = gh::FS.openWrite(path + ".tmp");
            else file = gh::FS.openWrite(path);
            if (!file) setError(gh::Error::OpenFile);
        } else {
            setError(gh::Error::FreeSpace);
        }

        if (!hasError()) answerCMD(Tag::upload_next);
        return !hasError();
    }

    void process(size_t typeHash, GHTREF data) {
        switch (typeHash) {
            case su::SH("crc"):
                setCRC(data);
                answerCMD(Tag::upload_next);
                break;
            case su::SH("next"):
                write64(data);
                if (!hasError()) answerCMD(Tag::upload_next);
                break;
            case su::SH("last"):
                lastChunk = 1;
                write64(data);
                end();
                if (!hasError()) answerCMD(Tag::upload_done);
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
        if (!file) return 0;
        uint16_t declen = data.sizeB64();
        uint8_t* buf = new uint8_t[declen];
        if (!buf) {
            setError(gh::Error::CantAlloc);
            return 0;
        }
        if (!data.decodeB64(buf, declen)) {
            setError(gh::Error::PacketDamage);
            delete[] buf;
            return 0;
        }
        bool ok = write(buf, declen);
        delete[] buf;
        return ok;
    }

    bool write(uint8_t* data, size_t len) {
        if (file) {
            tmr.restart();
            crc32 = ghc::crc32(crc32, data, len);
            upl_size -= len;
            if (file.write(data, len) != len) {
                setError(gh::Error::Write);
            }
        }
        return !hasError();
    }

    bool end() {
        if (file) {
            if (upl_size == 0 && upl_crc32 == crc32) {
                file.close();
                if (safe) {
                    gh::FS.remove(path);
                    gh::FS.rename(path + ".tmp", path);
                }
            } else {
                if (upl_crc32 != crc32) setError(gh::Error::CrcMiss);
                else setError(gh::Error::SizeMiss);
                cleanup();
            }
        }
        return !hasError();
    }

    bool timeout() {
        if (tmr) {
            setError(gh::Error::Timeout);
            cleanup();
        }
        return hasError();
    }

    void abort() {
        setError(gh::Error::Abort);
        cleanup();
    }

    static void sendError(gh::Client& client, uint32_t id, gh::Error err) {
        ghc::TransferBase::sendError(client, id, err, ghc::Tag::upload_err);
    }

    String path;
    uint32_t upl_crc32, crc32;

   private:
    bool safe;
    size_t upl_size;
    gh::Timer tmr;
    File file;
    bool lastChunk = 0;

    void cleanup() {
        if (file) {
            file.close();
            if (safe) gh::FS.remove(path + ".tmp");
            else gh::FS.remove(path);
        }
    }
};

}  // namespace ghc