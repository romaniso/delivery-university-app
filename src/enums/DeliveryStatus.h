// Status paczki na drodze od magazynu do odbiorcy:
// WAITING -> ASSIGNED -> IN_TRANSIT -> DELIVERED, a CANCELLED z WAITING lub ASSIGNED.

#pragma once

#include <stdexcept>
#include <string>

namespace enums {

enum class DeliveryStatus {
    WAITING,     // w magazynie, czeka w kolejce priorytetowej
    ASSIGNED,    // w kolejce kierowcy, kierowca jeszcze nie wyjechal
    IN_TRANSIT,  // kierowca wyjechal (zdarzenie DEPART)
    DELIVERED,   // dostarczona (zdarzenie DELIVERED)
    CANCELLED    // anulowana (zdarzenie DELIVERY_CANCELLED)
};

inline std::string toString(DeliveryStatus status) {
    switch (status) {
        case DeliveryStatus::WAITING: return "WAITING";
        case DeliveryStatus::ASSIGNED: return "ASSIGNED";
        case DeliveryStatus::IN_TRANSIT: return "IN_TRANSIT";
        case DeliveryStatus::DELIVERED: return "DELIVERED";
        case DeliveryStatus::CANCELLED: return "CANCELLED";
    }
    throw std::logic_error("nieznana wartosc DeliveryStatus");
}

}
