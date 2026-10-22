// Paczka - przechowywana w pamieci przez storage::Warehouse.
// Same dane, bez logiki (patrz README.md, src/model/).

#pragma once

#include <string>

#include "enums/DeliveryStatus.h"
#include "enums/Priority.h"

namespace model {

class Delivery {
public:
    // nadawane przez storage::Warehouse (1, 2, 3, ...)
    int id = 0;
    std::string address;
    double weightKg = 0.0;
    enums::Priority priority = enums::Priority::NORMAL;
    // dni numerowane od 1
    int arrivalDay = 0;
    int deadlineDay = 0;
    // kazda nowa paczka zaczyna w magazynie
    enums::DeliveryStatus status = enums::DeliveryStatus::WAITING;
    // 0 = jeszcze nie przydzielona
    int driverId = 0;
    // 0 = jeszcze nie dostarczona
    int deliveredDay = 0;
};

}
