#pragma once
#include <Arduino.h>

#include "hub_macro.hpp"

#ifndef GH_NO_FS
#ifdef GH_ESP_BUILD

#include <LittleFS.h>
#define GH_FS LittleFS

namespace ghc {

class HubFS {
   public:
    // read
    File openRead(const char* path) {
        return GH_FS.open(path, "r");
    }
    File openRead(const String& path) {
        return openRead(path.c_str());
    }

    // append
    File openAdd(const char* path) {
        return GH_FS.open(path, "a");
    }
    File openAdd(const String& path) {
        return openAdd(path.c_str());
    }

    // write
    File openWrite(const char* path) {
        return GH_FS.open(path, "w");
    }
    File openWrite(const String& path) {
        return openWrite(path.c_str());
    }

    // misc
    bool begin() {
#if GH_FS == LittleFS
#ifdef ESP32
        _mounted = GH_FS.begin(true);  // format if corrupted
#else
        _mounted = GH_FS.begin();
#endif  // ESP32

// not littlefs
#else
        _mounted = GH_FS.begin();
#endif
        return _mounted;
    }
    void end() {
        GH_FS.end();
        _mounted = 0;
    }
    bool mounted() {
        return _mounted;
    }
    void format() {
        GH_FS.format();
    }

    // remove
    bool remove(const char* path, bool remdir = true) {
        bool ok = GH_FS.remove(path);
        if (remdir) rmdir(path);
        return ok;
    }
    bool remove(const String& path, bool remdir = true) {
        return remove(path.c_str(), remdir);
    }

    // rename
    bool rename(const char* from, const char* to) {
        return GH_FS.rename(from, to);
    }
    bool rename(const String& from, const String& to) {
        return rename(from.c_str(), to.c_str());
    }

    // exists
    bool exists(const char* path) {
        return GH_FS.exists(path);
    }
    bool exists(const String& path) {
        return exists(path.c_str());
    }

    uint64_t freeSpace() {
#ifdef ESP8266
        FSInfo64 fs_info;
        GH_FS.info64(fs_info);
        return fs_info.totalBytes - fs_info.usedBytes;
#else
        return GH_FS.totalBytes() - GH_FS.usedBytes();
#endif
    }

    uint64_t totalSpace() {
#ifdef ESP8266
        FSInfo64 fs_info;
        GH_FS.info64(fs_info);
        return fs_info.totalBytes;
#else
        return GH_FS.totalBytes();
#endif
    }
    uint64_t usedSpace() {
#ifdef ESP8266
        FSInfo64 fs_info;
        GH_FS.info64(fs_info);
        return fs_info.usedBytes;
#else
        return GH_FS.usedBytes();
#endif
    }

    void mkdir(const String& path) {
        mkdir(path.c_str());
    }
    void mkdir(const char* path) {
#ifdef ESP32
        if (!GH_FS.exists(path)) {
            if (strchr(path, '/')) {
                char* pathStr = strdup(path);
                if (pathStr) {
                    char* ptr = strchr(pathStr, '/');
                    while (ptr) {
                        *ptr = 0;
                        GH_FS.mkdir(pathStr);
                        *ptr = '/';
                        ptr = strchr(ptr + 1, '/');
                    }
                }
                free(pathStr);
            }
        }
#else
#endif
    }
    void rmdir(const String& path) {
        rmdir(path.c_str());
    }
    void rmdir(const char* path) {
#ifdef ESP32
        char* pathStr = strdup(path);
        if (pathStr) {
            char* ptr = strrchr(pathStr, '/');
            while (ptr) {
                *ptr = 0;
                GH_FS.rmdir(pathStr);
                ptr = strrchr(pathStr, '/');
            }
            free(pathStr);
        }
#else
#endif
    }
    void listDir(String& str, const String& path, char div) {
        str = "";
#ifdef ESP8266
        Dir dir = GH_FS.openDir(path);
        while (dir.next()) {
            if (dir.isFile() && dir.fileName().length()) {
                str += path;
                str += dir.fileName();
                str += div;
            }
        }
#else  // ESP32
        File root = GH_FS.open(path.c_str());
        if (!root || !root.isDirectory()) return;
        File file;
        while (file = root.openNextFile()) {
            if (!file.isDirectory()) {
                str += path;
                str += '/';
                str += file.name();
                str += div;
            }
        }
#endif
        if (str.length()) str.remove(str.length() - 1);
    }
    void showFiles(String& answ, const String& path, GH_UNUSED uint8_t levels = 0, uint32_t* count = nullptr) {
#ifdef ESP8266
        Dir dir = GH_FS.openDir(path);
        while (dir.next()) {
            if (dir.isDirectory()) {
                String p(path);
                p += dir.fileName();
                p += '/';
                answ += '\"';
                answ += p;
                answ += "\":0,";
                if (count) {
                    *count += answ.length();
                    answ = "";
                }
                Dir sdir = GH_FS.openDir(p);
                HubFS::showFiles(answ, p);
            }
            if (dir.isFile() && dir.fileName().length()) {
                answ += '\"';
                answ += path;
                answ += dir.fileName();
                answ += "\":";
                answ += dir.fileSize();
                answ += ',';
                if (count) {
                    *count += answ.length();
                    answ = "";
                }
            }
        }

#else  // ESP32
        File root = GH_FS.open(path.c_str());
        if (!root || !root.isDirectory()) return;
        File file;
        while (file = root.openNextFile()) {
            if (file.isDirectory()) {
                answ += '\"';
                answ += file.path();
                answ += "/\":0,";
                if (count) {
                    *count += answ.length();
                    answ = "";
                }
                if (levels) HubFS::showFiles(answ, file.path(), levels - 1);
            } else {
                answ += '\"';
                if (levels != GH_FS_DEPTH) answ += path;
                answ += '/';
                answ += file.name();
                answ += "\":";
                answ += file.size();
                answ += ',';
                if (count) {
                    *count += answ.length();
                    answ = "";
                }
            }
        }
#endif
    }

   private:
    bool _mounted = 0;
};

}  // namespace ghc

namespace gh {
ghc::HubFS FS __attribute__((weak));
}

#endif  // GH_ESP_BUILD
#endif  // GH_NO_FS