// Magazyn - trzyma wszystkie dane programu w pamieci (RAM).
// Dane wczytuje io::InputFile, wynik zapisuje io::OutputFile.

#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "model/Delivery.h"
#include "model/Driver.h"

namespace storage {

class Warehouse {
public:
    // Nadaje kierowcy kolejne id (1, 2, 3, ...) i zapisuje go
    void addDriver(model::Driver driver) {
        driver.id = nextDriverId_++;
        drivers_.push_back(driver);
    }

    // Nadaje paczce kolejne id (1, 2, 3, ...) i zapisuje ja
    void addDelivery(model::Delivery delivery) {
        delivery.id = nextDeliveryId_++;
        deliveries_.push_back(delivery);
    }

    const std::vector<model::Driver>& drivers() const { return drivers_; }
    const std::vector<model::Delivery>& deliveries() const { return deliveries_; }

    // Wyszukiwanie po id. Id nadajemy po kolei i nic nie usuwamy,
    // wiec element o id n lezy pod indeksem n - 1.
    // Zwraca referencje - zmiany (np. status paczki) trafiaja prosto do magazynu.
    model::Driver& driver(int id) {
        checkId(id, drivers_.size(), "kierowcy");
        return drivers_[id - 1];
    }

    model::Delivery& delivery(int id) {
        checkId(id, deliveries_.size(), "paczki");
        return deliveries_[id - 1];
    }

private:
    std::vector<model::Driver> drivers_;
    std::vector<model::Delivery> deliveries_;
    int nextDriverId_ = 1;
    int nextDeliveryId_ = 1;

    static void checkId(int id, std::size_t count, const std::string& what) {
        if (id < 1 || static_cast<std::size_t>(id) > count) {
            throw std::out_of_range("nie ma " + what + " o id " + std::to_string(id));
        }
    }
};

}  // namespace storage
