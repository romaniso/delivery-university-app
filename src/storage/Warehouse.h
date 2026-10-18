// Magazyn - trzyma wszystkie dane programu w pamieci (RAM).
// Dane wczytuje io::InputFile, wynik zapisuje io::OutputFile.

#pragma once

#include <vector>

#include "model/Driver.h"

namespace storage {

class Warehouse {
public:
    // Nadaje kierowcy kolejne id (1, 2, 3, ...) i zapisuje go
    void addDriver(model::Driver driver) {
        driver.id = nextDriverId_++;
        drivers_.push_back(driver);
        // przyjmuje z consoli
    }

    const std::vector<model::Driver>& drivers() const { return drivers_; }

private:
    std::vector<model::Driver> drivers_;
    int nextDriverId_ = 1;
};

}  // namespace storage
