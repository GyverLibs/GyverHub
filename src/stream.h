#pragma once
#include "config.hpp"
#include "macro.hpp"

#ifdef GH_NO_STREAM
class HubStream {
   public:
};
#else

#include <Arduino.h>
#include <Stream.h>

#include "utils/stats.h"

class HubStream {
   public:
    // подключить Stream для связи
    void setupStream(Stream* nstream, GHconn_t nfrom) {
        stream = nstream;
        from = nfrom;
    }

   protected:
    void tickStream() {
        if (stream && stream->available()) {
            String str = stream->readStringUntil('\0');
            parse((char*)str.c_str(), from, GH_STREAM);
        }
    }

    void sendStream(const String& answ) {
        if (stream) stream->print(answ);
    }

    bool stateStream() {
        return stream;
    }
    GHconn_t connStream() {
        return from;
    }

    virtual void parse(char* url, GHconn_t from, GHsource_t source) = 0;

    // ============ PRIVATE =============
   private:
    Stream* stream = nullptr;
    GHconn_t from;
};
#endif