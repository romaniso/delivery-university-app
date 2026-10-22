// Kurs kierowcy w jednym dniu - paczki w kolejnosci doreczania.
// Zapisywany przy doreczeniu, bo kolejka kierowcy jest wtedy oprozniana,
// a raport potrzebuje tej kolejnosci (lista adresow).
// Same dane, bez logiki (patrz README.md, src/model/).

#pragma once

#include <vector>

namespace model {

class DriverDay {
public:
    int driverId = 0;
    int day = 0;
    // id paczek w kolejnosci z kolejki kierowcy
    std::vector<int> deliveryIds;
    double loadKg = 0.0;
};

}
