#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "bridges/esp/sync/http_class.h"
#include "core/client.h"
#include "core/core_class.h"
#include "core/fs.h"
#include "core/hooks.h"
#include "core/hub_class.h"
#include "core/packet.h"
#include "core/types.h"
#include "core/utils/crc32.h"
#include "hub_macro.hpp"
#include "transfer.h"
#include "ui/timer.h"

namespace gh {
class Fetcher;
}

namespace ghc {
typedef void (*FetchHook)(void* hubptr, gh::Fetcher* fetcher);
}

namespace gh {

#if !defined(GH_NO_FS) && (!defined(GH_NO_FETCH) || !defined(GH_NO_HTTP_FETCH))
class Fetcher : private ghc::TransferBase {
    friend class ghc::HubCore;
    friend class ghc::HubHTTP;
    friend class ::GyverHub;

   public:
    using ghc::TransferBase::client;

    Fetcher(Client& client,
            ghc::FetchHook hook,
            GHTREF path,
            uint32_t id = 0) : ghc::TransferBase(client, id, ghc::Tag::fetch_err), fetch_h(hook) {
        path.toString(pathStr);
        this->path = GHTXT(pathStr);
    }

    // отправить файл по пути
    void fetchFile(const char* path) {
        cleanup();
        file = FS.openRead(path);
        if (file) len = file.size();
    }

    // отправить файл
    template <typename T>
    void fetchFile(T& file) {
        cleanup();
        this->file = file;
        if (file) len = file.size();
    }

    // отправить сырые данные
    void fetchBytes(uint8_t* bytes, size_t len) {
        cleanup();
        this->bytes = bytes;
        this->len = len;
        pgm = false;
    }

    // отправить сырые данные из PGM
    void fetchBytes_P(const uint8_t* bytes, size_t len) {
        cleanup();
        this->bytes = bytes;
        this->len = len;
        pgm = true;
    }

    // true - начало скачивания
    bool start;

    // путь к скачиваемому файлу
    GHTXT path;

   private:
    bool begin() {
        tmr.setTime(GH_CONN_TOUT * 1000);
        tmr.startTimeout();
        start = 1;
        hook();

        if (!file && !bytes) {
            file = FS.openRead(pathStr);
            if (file) len = file.size();
        }

        if (!file && !bytes) {
            setError(Error::Start);
        } else {
            if (canSend()) {
                ghc::Packet p(50, &client);
                p.beginPacket(id);
                p.addString(ghc::Tag::type, ghc::Tag::fetch_start);
                p.addInt(ghc::Tag::len, len);
                p.endPacket();
                p.send();
            }
        }
        return !hasError();
    }

    // true == last
    bool next() {
        bool last = 0;
        if (canSend()) {
            ghc::Packet p(50 + GH_FETCH_CHUNK_SIZE, &client);
            p.beginPacket(id);
            p.addString(ghc::Tag::type, ghc::Tag::fetch_chunk);
            p.addKey(ghc::Tag::data);
            p.quotes();
            last = writeChunk(p);
            p.quotes();
            p.comma();
            if (last) {
                p.addInt(ghc::Tag::last, 1);
                p.addInt(ghc::Tag::crc32, crc32);
            }
            p.endPacket();
            p.send();
        }

        if (last) end();
        return last;
    }

    void end() {
        start = 0;
        hook();
        cleanup();
    }

    // true == last
    bool writeChunk(ghc::Packet& p) {
        bool lastChunk = 0;
        tmr.restart();
        if (file) {
            uint8_t* buf = new uint8_t[GH_FETCH_CHUNK_SIZE * 3 / 4];
            if (!buf) {
                setError(gh::Error::CantAlloc);
                end();
                return 1;
            }
            size_t rlen = file.readBytes((char*)buf, sizeof(buf));
            if (sizeof(buf) != rlen) lastChunk = 1;
            su::b64::encode(&p.s, buf, rlen);
            crc32 = ghc::crc32(crc32, buf, rlen);
            delete[] buf;
        } else if (bytes) {
            size_t rlen = GH_FETCH_CHUNK_SIZE * 3 / 4;
            if (len >= rlen) {
                len -= rlen;
                if (!len) lastChunk = 1;
            } else {
                rlen = len;
                lastChunk = 1;
            }
            su::b64::encode(&p.s, (uint8_t*)bytes + b_index, rlen, pgm);
            crc32 = ghc::crc32(crc32, (uint8_t*)bytes + b_index, rlen, pgm);
            b_index += rlen;
        }
        return lastChunk;
    }

    bool timeout() {
        if (tmr) {
            setError(Error::Timeout);
            end();
        }
        return hasError();
    }

    void abort() {
        setError(Error::Abort);
        end();
    }

    static void sendError(gh::Client& client, uint32_t id, gh::Error err) {
        ghc::TransferBase::sendError(client, id, err, ghc::Tag::fetch_err);
    }

   private:
    ghc::FetchHook fetch_h;
    String pathStr;

    Timer tmr;
    File file;
    bool pgm = false;
    const uint8_t* bytes = nullptr;
    size_t len = 0;
    uint32_t b_index = 0;
    uint32_t crc32 = 0;

    void cleanup() {
        if (file) file.close();
        bytes = nullptr;
        len = 0;
        b_index = 0;
    }
    void hook() {
        fetch_h(client._hub, this);
    }
};
#endif

}  // namespace gh