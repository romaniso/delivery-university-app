// Kolejka paczek jednego kierowcy na dany dzien.
// Same dane, bez logiki (patrz README.md, src/model/).

#pragma once

#include <queue>

#include "enums/QueueStatus.h"

namespace model {

class DriverQueue {
public:
    int driverId = 0;
    // id paczek (dane paczek sa w storage::Warehouse), w kolejnosci ze strategii -
    // kierowca dostaje gotowa kolejke i dorecza od poczatku
    std::queue<int> deliveries;
    // DEPARTED = kierowca wyjechal, nic juz nie dokladamy
    enums::QueueStatus status = enums::QueueStatus::OPEN;
    // suma wag przydzielonych paczek - nie moze przekroczyc capacityKg kierowcy
    double loadKg = 0.0;
};

}
