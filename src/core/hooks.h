#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "bridge_class.h"
#include "client_class.h"
#include "hub_macro.hpp"
#include "request_class.h"
#include "types.h"

namespace ghc {

typedef void (*ParseHook)(void* hub, gh::Bridge& bridge, GHTREF url, GHTREF data);
typedef void (*SendHook)(void* hub, GHTREF text, gh::Client* client);

#ifdef GH_ESP_BUILD
typedef bool (*RequestHook)(void* hubptr, gh::Request* request);
typedef void (*UploadHook)(void* hubptr, String& path);
#endif

}  // namespace ghc