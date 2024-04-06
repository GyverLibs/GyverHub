#include <Arduino.h>

#include "client.h"
#include "hub_macro.hpp"
#include "request_class.h"
#include "types.h"
#include "ui/builder_class.h"
#include "ui/data.h"
#include "ui/info_class.h"
#include "ui/location.h"

#ifdef GH_ESP_BUILD
#include "transfer/fetcher.h"
#endif

namespace ghc {

#ifdef GH_ESP_BUILD
#include <functional>
typedef std::function<void(gh::Client& client)> PingCallback;
typedef std::function<void(gh::Data data)> DataCallback;
typedef std::function<void(String str)> CliCallback;
typedef std::function<void(uint32_t stamp)> UnixCallback;
typedef std::function<void(gh::Location location)> LocationCallback;
typedef std::function<void(gh::Builder& builder)> BuildCallback;
typedef std::function<bool(gh::Request& request)> RequestCallback;
typedef std::function<void(gh::Info& info)> InfoCallback;

typedef std::function<void(gh::Reboot res)> RebootCallback;
typedef std::function<void(gh::Fetcher& fetcher)> FetchCallback;
typedef std::function<void(String& path)> UploadCallback;
#else
typedef void (*PingCallback)(gh::Client& client);
typedef void (*DataCallback)(gh::Data data);
typedef void (*CliCallback)(String str);
typedef void (*UnixCallback)(uint32_t stamp);
typedef void (*LocationCallback)(gh::Location location);
typedef void (*BuildCallback)(gh::Builder& builder);
typedef bool (*RequestCallback)(gh::Request& request);
typedef void (*InfoCallback)(gh::Info& info);
#endif

}  // namespace ghc