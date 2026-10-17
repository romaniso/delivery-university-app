// Kierowca - odpowiada wierszowi tabeli Driver.
// Same dane, bez logiki (patrz README.md, src/model/).

#pragma once

#include <string>

namespace model {

class Driver {
public:
    // primary key
    int id = 0;
    std::string name;
    double capacityKg = 0.0;
    bool isAvailable = true;
};

}
