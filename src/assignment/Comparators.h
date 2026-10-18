// Komparatory / funktory dla std::sort i std::priority_queue.

#pragma once

#include "model/Driver.h"

namespace assignment {

// Dla std::sort: najpierw kierowcy o najwiekszej ladownosci.
// Przy rownej ladownosci decyduje id, zeby kolejnosc byla zawsze taka sama.
struct DriverCapacityGreater {
    bool operator()(const model::Driver& a, const model::Driver& b) const {
        if (a.capacityKg != b.capacityKg) {
            return a.capacityKg > b.capacityKg;
        }
        return a.id < b.id;
    }
};

// TODO: komparator score paczek dla std::priority_queue (razem z klasa Delivery)

}  // namespace assignment
