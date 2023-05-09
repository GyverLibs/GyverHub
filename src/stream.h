#pragma once
#include "config.h"
#include "macro.h"

#ifdef GH_NO_STREAM
class HubStream {
   public:
};
#else

#include <Arduino.h>
#include <Stream.h>

#include "utils/stats.h"

class HubStream {
    // ============ PUBLIC =============
   public:
    // подключить Stream (например &Serial, &Serial1, &mySerial...)
    void setupStream(Stream* nstream) {
        stream = nstream;
    }

    // ============ PROTECTED =============
   protected:
    virtual void parse(char* url, GHconn_t conn) = 0;

    void tickStream() {
        if (stream && stream->available()) {
            String s = stream->readStringUntil('\n');
            parse((char*)s.c_str(), GH_STREAM);
        }
    }

    void sendStream(const String& answ) {
        if (stream) stream->print(answ);
    }

    // ============ PRIVATE =============
   private:
    Stream* stream = nullptr;
};
#endif