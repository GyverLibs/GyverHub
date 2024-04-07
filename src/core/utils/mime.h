#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "hub_macro.hpp"

namespace ghc {

String __attribute__((weak)) getMime(GHTXT path) {
    int16_t pos = path.lastIndexOf('.');
    if (pos > 0) {
        path = path.substring(pos + 1);
        switch (path.hash()) {
            case su::SH("avi"): return F("video/x-msvideo");
            case su::SH("bin"): return F("application/octet-stream");
            case su::SH("bmp"): return F("image/bmp");
            case su::SH("css"): return F("text/css");
            case su::SH("csv"): return F("text/csv");
            case su::SH("gz"): return F("application/gzip");
            case su::SH("gif"): return F("image/gif");
            case su::SH("html"): return F("text/html");
            case su::SH("jpeg"):
            case su::SH("jpg"): return F("image/jpeg");
            case su::SH("js"): return F("text/javascript");
            case su::SH("json"): return F("application/json");
            case su::SH("png"): return F("image/png");
            case su::SH("svg"): return F("image/svg+xml");
            case su::SH("wav"): return F("audio/wav");
            case su::SH("xml"): return F("application/xml");
        }
    }
    return F("text/plain");
}

}  // namespace ghc