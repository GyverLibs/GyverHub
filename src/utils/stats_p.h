#pragma once
#include <Arduino.h>
#include "../config.hpp"
#include "../macro.hpp"
#include "stats.h"
#include "action.h"
#include "build.h"

// получить текстовое значение типа GHevent_t для вывода в порт
FSTR GHreadEvent(GHevent_t n);

// получить текстовое значение типа GHconn_t для вывода в порт
FSTR GHreadConn(GHconn_t n);

// получить текстовое значение типа GHbuild_t для вывода в порт
FSTR GHreadBuild(GHbuild_t n);

// получить текстовое значение типа GHreason_t для вывода в порт
FSTR GHreadReason(GHreason_t n);