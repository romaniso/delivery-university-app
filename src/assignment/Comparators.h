// Komparatory / funktory dla std::sort i std::priority_queue.

#pragma once

#include "assignment/DeliveryScore.h"
#include "enums/DeliveryStatus.h"
#include "enums/Priority.h"
#include "model/Delivery.h"
#include "model/Driver.h"
#include "storage/Warehouse.h"

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

// ---------------------------------------------------------------------------
// Strategie kolejnosci paczek - funktory dla std::priority_queue<int, ...> z id paczek.
// operator()(a, b) zwraca true, gdy paczka a ma byc NIZEJ niz b (na gorze jest "najwieksza").
//
// Kazda strategia porownuje w tej samej kolejnosci kluczy (README.md, "Score i strategie"):
//   1. POSTPONED przed WAITING,
//   2. klucz strategii,
//   3. remis: wczesniejszy arrivalDay, potem mniejsze id.
// ---------------------------------------------------------------------------

inline bool isPostponed(const model::Delivery& delivery) {
    return delivery.status == enums::DeliveryStatus::POSTPONED;
}

// Klucz 3: a nizej niz b, gdy przyszla pozniej (przy tym samym dniu - wieksze id)
inline bool fifoBelow(const model::Delivery& a, const model::Delivery& b) {
    if (a.arrivalDay != b.arrivalDay) {
        return a.arrivalDay > b.arrivalDay;
    }
    return a.id > b.id;
}

// Strategia A - kolejnosc przyjecia (klucz 2 to od razu klucz 3: arrivalDay, potem id).
struct FifoOrder {
    const storage::Warehouse& warehouse;

    bool operator()(int aId, int bId) const {
        const model::Delivery& a = warehouse.delivery(aId);
        const model::Delivery& b = warehouse.delivery(bId);
        if (isPostponed(a) != isPostponed(b)) {
            return isPostponed(b);
        }
        return fifoBelow(a, b);
    }
};

// Strategia B - sam priorytet: URGENT przed NORMAL.
struct PriorityOrder {
    const storage::Warehouse& warehouse;

    bool operator()(int aId, int bId) const {
        const model::Delivery& a = warehouse.delivery(aId);
        const model::Delivery& b = warehouse.delivery(bId);
        if (isPostponed(a) != isPostponed(b)) {
            return isPostponed(b);
        }
        if (a.priority != b.priority) {
            return a.priority == enums::Priority::NORMAL;
        }
        return fifoBelow(a, b);
    }
};

// Strategia C - pelny score w danym dniu.
// Kolejke budujemy od nowa kazdego dnia, bo score zmienia sie z dniem (aging, termin).
struct ScoreOrder {
    const storage::Warehouse& warehouse;
    int today;

    bool operator()(int aId, int bId) const {
        const model::Delivery& a = warehouse.delivery(aId);
        const model::Delivery& b = warehouse.delivery(bId);
        if (isPostponed(a) != isPostponed(b)) {
            return isPostponed(b);
        }
        int aScore = deliveryScore(a, today);
        int bScore = deliveryScore(b, today);
        if (aScore != bScore) {
            return aScore < bScore;
        }
        return fifoBelow(a, b);
    }
};

}  // namespace assignment
