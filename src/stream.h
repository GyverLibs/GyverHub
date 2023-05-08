#pragma once
#include <Arduino.h>
#include <Stream.h>

#include "config.h"
#include "macro.h"
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
        }
    }

    void sendStream(const String& answ) {
      if (stream) stream->println(answ);
    }

    // ============ PRIVATE =============
   private:
    Stream* stream = nullptr;
};