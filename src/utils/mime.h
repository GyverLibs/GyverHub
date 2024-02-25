#pragma once
#include <Arduino.h>
#include <StringUtils.h>

namespace ghc {

String __attribute__((weak)) getMime(const String &path) {
    int16_t pos = path.lastIndexOf('.');
    if (pos > 0) {
        const char* ext = path.c_str() + pos + 1;
        switch (sutil::hash(ext)) {
            case sutil::SH("avi"): return F("video/x-msvideo");
            case sutil::SH("bin"): return F("application/octet-stream");
            case sutil::SH("bmp"): return F("image/bmp");
            case sutil::SH("css"): return F("text/css");
            case sutil::SH("csv"): return F("text/csv");
            case sutil::SH("gz"): return F("application/gzip");
            case sutil::SH("gif"): return F("image/gif");
            case sutil::SH("html"): return F("text/html");
            case sutil::SH("jpeg"):
            case sutil::SH("jpg"): return F("image/jpeg");
            case sutil::SH("js"): return F("text/javascript");
            case sutil::SH("json"): return F("application/json");
            case sutil::SH("png"): return F("image/png");
            case sutil::SH("svg"): return F("image/svg+xml");
            case sutil::SH("wav"): return F("audio/wav");
            case sutil::SH("xml"): return F("application/xml");
        }
    }
    return F("text/plain");
}

}  // namespace ghc