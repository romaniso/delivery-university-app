// Magazyn - trzyma wszystkie dane programu w pamieci (RAM).
// Dane wczytuje io::InputFile, wynik zapisuje io::OutputFile.

#pragma once

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
    // at() sprawdza indeks - zly id rzuca std::out_of_range.
    model::Driver& driver(int id) { return drivers_.at(id - 1); }
    model::Delivery& delivery(int id) { return deliveries_.at(id - 1); }

    // Wersje tylko do odczytu - np. dla strategii, ktore porownuja paczki
    const model::Driver& driver(int id) const { return drivers_.at(id - 1); }
    const model::Delivery& delivery(int id) const { return deliveries_.at(id - 1); }

private:
    std::vector<model::Driver> drivers_;
    std::vector<model::Delivery> deliveries_;
    int nextDriverId_ = 1;
    int nextDeliveryId_ = 1;
};

}  // namespace storage
