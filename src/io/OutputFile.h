// Zapis raportu do pliku wyjsciowego (patrz README.md, "Plik wyjsciowy").

#pragma once

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "model/Driver.h"

namespace io {

class OutputFile {
public:
    explicit OutputFile(std::string path) : path_(std::move(path)) {}

    // Kierowcy trafiaja do raportu w podanej kolejnosci - sortuje wywolujacy
    void write(const std::vector<model::Driver>& drivers) const {
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

        // TODO: przydzial paczek do kierowcow, paczki pozostale w magazynie
    }

private:
    std::string path_;
};

}  // namespace io
