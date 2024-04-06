#pragma once
#include <Arduino.h>

#include "core/client.h"
#include "hub_macro.hpp"
#include "transfer.h"

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#else
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#endif

namespace ghc {

class OtaUrl : public ghc::TransferBase {
   public:
    OtaUrl(size_t typeHash,
           GHTREF url,
           gh::Client& client,
           uint32_t id) : ghc::TransferBase(client, id, ghc::Tag::ota_url_err),
                             url(url.toString()),
                             typeHash(typeHash) {}

    bool update() {
        bool ok = 0;
#ifdef ESP8266
        ESPhttpUpdate.rebootOnUpdate(false);
        BearSSL::WiFiClientSecure wclient;
        if (url.startsWith(F("https"))) wclient.setInsecure();
        switch (typeHash) {
            case su::SH("flash"):
                ok = ESPhttpUpdate.update(wclient, url);
                break;
            case su::SH("fs"):
                ok = ESPhttpUpdate.updateFS(wclient, url);
                break;
            default:
                sendError(client, id, gh::Error::WrongType);
                return 0;
                break;
        }
#else
        httpUpdate.rebootOnUpdate(false);
        WiFiClientSecure wclient;
        if (url.startsWith(F("https"))) wclient.setInsecure();
        switch (typeHash) {
            case su::SH("flash"):
                ok = httpUpdate.update(wclient, url);
                break;
            case su::SH("fs"):
                ok = httpUpdate.updateSpiffs(wclient, url);
                break;
            default:
                sendError(client, id, gh::Error::WrongType);
                return 0;
                break;
        }
#endif
        if (ok) answerCMD(Tag::ota_url_ok);
        else sendError(client, id, gh::Error::End);
        return ok;
    }

    static void sendError(gh::Client& client, uint32_t id, gh::Error err) {
        ghc::TransferBase::sendError(client, id, err, ghc::Tag::ota_url_err);
    }

   private:
    String url;
    size_t typeHash;
};

}  // namespace ghc