#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "client.h"
#include "geo.h"
#include "hub_macro.hpp"

namespace gh {

class Location : public Geo {
   public:
    Location(GHTREF stamp, GHTREF data, Client& client) : client(client) {
        this->stamp = stamp;
        float* loc[] = {&lat, &lon, &alt, &speed, &heading, &accuracy};
        data.split(loc, 6, ';');
    }

    // float lat;
    // float lon;

    // время, когда получена позиция
    uint32_t stamp = 0;

    // Высота над уровнем моря в метрах
    float alt = 0;

    // Скорость движения в м/с
    float speed = 0;

    // Направление движения в градусах от севера по часовой стрелке
    float heading = 0;

    // Точность измерений в метрах
    float accuracy = 0;

    // клиент
    Client& client;

   private:
};

}  // namespace gh