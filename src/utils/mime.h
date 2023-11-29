#pragma once

#include <Arduino.h>
#include "../macro.hpp"

#define GH_MIME_AMOUNT 17

GH_PGM(_GH_MIME_EX0, ".avi");
GH_PGM(_GH_MIME_EX1, ".bin");
GH_PGM(_GH_MIME_EX2, ".bmp");
GH_PGM(_GH_MIME_EX3, ".css");
GH_PGM(_GH_MIME_EX4, ".csv");
GH_PGM(_GH_MIME_EX5, ".gz");
GH_PGM(_GH_MIME_EX6, ".gif");
GH_PGM(_GH_MIME_EX7, ".html");
GH_PGM(_GH_MIME_EX8, ".jpeg");
GH_PGM(_GH_MIME_EX9, ".jpg");
GH_PGM(_GH_MIME_EX10, ".js");
GH_PGM(_GH_MIME_EX11, ".json");
GH_PGM(_GH_MIME_EX12, ".png");
GH_PGM(_GH_MIME_EX13, ".svg");
GH_PGM(_GH_MIME_EX14, ".txt");
GH_PGM(_GH_MIME_EX15, ".wav");
GH_PGM(_GH_MIME_EX16, ".xml");
GH_PGM_LIST(_GH_mimie_ex_list, _GH_MIME_EX0, _GH_MIME_EX1, _GH_MIME_EX2, _GH_MIME_EX3, _GH_MIME_EX4, _GH_MIME_EX5, _GH_MIME_EX6, _GH_MIME_EX7, _GH_MIME_EX8, _GH_MIME_EX9, _GH_MIME_EX10, _GH_MIME_EX11, _GH_MIME_EX12, _GH_MIME_EX13, _GH_MIME_EX14, _GH_MIME_EX15, _GH_MIME_EX16);

GH_PGM(_GH_MIME0, "video/x-msvideo");
GH_PGM(_GH_MIME1, "application/octet-stream");
GH_PGM(_GH_MIME2, "image/bmp");
GH_PGM(_GH_MIME3, "text/css");
GH_PGM(_GH_MIME4, "text/csv");
GH_PGM(_GH_MIME5, "application/gzip");
GH_PGM(_GH_MIME6, "image/gif");
GH_PGM(_GH_MIME7, "text/html");
GH_PGM(_GH_MIME8, "image/jpeg");
GH_PGM(_GH_MIME9, "image/jpeg");
GH_PGM(_GH_MIME10, "text/javascript");
GH_PGM(_GH_MIME11, "application/json");
GH_PGM(_GH_MIME12, "image/png");
GH_PGM(_GH_MIME13, "image/svg+xml");
GH_PGM(_GH_MIME14, "text/plain");
GH_PGM(_GH_MIME15, "audio/wav");
GH_PGM(_GH_MIME16, "application/xml");
GH_PGM_LIST(_GH_mimie_list, _GH_MIME0, _GH_MIME1, _GH_MIME2, _GH_MIME3, _GH_MIME4, _GH_MIME5, _GH_MIME6, _GH_MIME7, _GH_MIME8, _GH_MIME9, _GH_MIME10, _GH_MIME11, _GH_MIME12, _GH_MIME13, _GH_MIME14, _GH_MIME15, _GH_MIME16);