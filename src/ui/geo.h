#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "flag.h"

namespace gh {

class Geo : public Flag {
   public:
    Geo() {}
    Geo(const su::Text& text, bool change = 0) : Flag(change) {
        float* latlon[] = {&lat, &lon};
        text.split(latlon, 2, ';');
    }
    Geo(float lat, float lon, bool change = 0) : Flag(change), lat(lat), lon(lon) {}

    // широта
    float lat = 0;

    // долгота
    float lon = 0;

    // bool changed();

    // расстояние до другой точки в метрах
    float dist(const Geo& geo) const {
        // https://en.wikipedia.org/wiki/Haversine_formula
        float R = 6378.137;
        float dLat = (geo.lat - lat) * PI / 180;
        float dLon = (geo.lon - lon) * PI / 180;

        float a = _sq(sin(dLat / 2)) + cos(lat * PI / 180) * cos(geo.lat * PI / 180) * _sq(sin(dLon / 2));
        return R * atan2(sqrt(a), sqrt(1 - a)) * 2000;

        // geo.lat -= lat;
        // geo.lon -= lon;
        // return sqrt(geo.lat * geo.lat + geo.lon * geo.lon);
    }

    // lat * 1000000
    long _latE6() const {
        return lat * 1000000;
    }

    // lon * 1000000
    long _lonE6() const {
        return lon * 1000000;
    }

   private:
    float _sq(float v) const {
        return v * v;
    }
};

}  // namespace gh