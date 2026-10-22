// Status paczki na drodze od magazynu do odbiorcy:
// WAITING -> ASSIGNED -> IN_TRANSIT -> DELIVERED,
// a gdy paczka nie zmiesci sie w dzisiejszym przydziale: WAITING -> POSTPONED -> ASSIGNED -> ...

#pragma once

#include <string>

namespace enums {

enum class DeliveryStatus {
    WAITING,     // w magazynie, dzis przyjeta - jeszcze nie bylo proby przydzialu
    POSTPONED,   // w magazynie, nie zmiescila sie w ktoryms poprzednim przydziale - przed WAITING
    ASSIGNED,    // w kolejce kierowcy, kierowca jeszcze nie wyjechal
    IN_TRANSIT,  // kierowca wyjechal (zdarzenie DEPART)
    DELIVERED    // dostarczona (zdarzenie DELIVERED)
};

inline std::string toString(DeliveryStatus status) {
    switch (status) {
        case DeliveryStatus::WAITING: return "WAITING";
        case DeliveryStatus::POSTPONED: return "POSTPONED";
        case DeliveryStatus::ASSIGNED: return "ASSIGNED";
        case DeliveryStatus::IN_TRANSIT: return "IN_TRANSIT";
        case DeliveryStatus::DELIVERED: return "DELIVERED";
    }
    return "?";  // nie powinno sie zdarzyc - kazda wartosc jest obsluzona wyzej
}

}
