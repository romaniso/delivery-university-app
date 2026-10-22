// Zapis raportu do pliku wyjsciowego (patrz README.md, "Plik wyjsciowy").

#pragma once

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "enums/DeliveryStatus.h"
#include "enums/Priority.h"
#include "model/Delivery.h"
#include "model/Driver.h"

namespace io {

class OutputFile {
public:
    explicit OutputFile(std::string path) : path_(std::move(path)) {}

    // Kierowcy i paczki trafiaja do raportu w podanej kolejnosci - sortuje wywolujacy
    void write(const std::vector<model::Driver>& drivers,
               const std::vector<model::Delivery>& deliveries) const {
        std::ofstream file(path_);
        if (!file) {
            throw std::runtime_error("Nie mozna zapisac pliku wyjsciowego: " + path_);
        }

        file << "Kolejka Paczek - raport\n\n";
        file << "Kierowcy (wg ladownosci):\n";
        file << std::left << std::setw(4) << "id" << std::setw(24) << "imie i nazwisko"
             << std::right << std::setw(12) << "ladownosc" << "  dostepny\n";
        for (const model::Driver& driver : drivers) {
            file << std::left << std::setw(4) << driver.id << std::setw(24) << driver.name
                 << std::right << std::setw(9) << std::fixed << std::setprecision(1)
                 << driver.capacityKg << " kg" << "  " << (driver.isAvailable ? "tak" : "nie")
                 << '\n';
        }

        file << "\nPaczki:\n";
        file << std::left << std::setw(4) << "id" << std::setw(32) << "adres" << std::right
             << std::setw(12) << "waga" << "  " << std::left << std::setw(10) << "priorytet"
             << std::right << std::setw(9) << "przyjeta" << std::setw(8) << "termin" << "  "
             << "status\n";
        for (const model::Delivery& delivery : deliveries) {
            file << std::left << std::setw(4) << delivery.id << std::setw(32) << delivery.address
                 << std::right << std::setw(9) << std::fixed << std::setprecision(1)
                 << delivery.weightKg << " kg" << "  " << std::left << std::setw(10)
                 << enums::toString(delivery.priority) << std::right << std::setw(9)
                 << delivery.arrivalDay << std::setw(8) << delivery.deadlineDay << "  "
                 << enums::toString(delivery.status) << '\n';
        }

        // TODO: przydzial paczek do kierowcow (listy adresow), paczki pozostale w magazynie
    }

private:
    std::string path_;
};

}  // namespace io
