// Odczyt pliku wejsciowego do magazynu (format: patrz README.md, "Plik wejsciowy").

#pragma once

#include <cstddef>
#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "model/Driver.h"
#include "storage/Warehouse.h"

namespace io {

class InputFile {
public:
    explicit InputFile(std::string path) : path_(std::move(path)) {}

    // Wczytuje wszystkie rekordy do magazynu. Przy bledzie rzuca std::runtime_error
    // z numerem linii.
    void loadInto(storage::Warehouse& warehouse) const {
        std::ifstream file(path_);
        if (!file) {
            throw std::runtime_error("Nie mozna otworzyc pliku wejsciowego: " + path_);
        }

        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();  // plik zapisany na Windows
            }
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::vector<std::string> fields = split(line, ';');
            if (fields[0] == "DRIVER") {
                warehouse.addDriver(parseDriver(fields, lineNumber));
            } else {
                throw error(lineNumber, "nieznany typ rekordu \"" + fields[0] + "\"");
            }
        }
    }

private:
    std::string path_;

    static std::vector<std::string> split(const std::string& line, char separator) {
        std::vector<std::string> fields;
        std::stringstream stream(line);
        std::string field;
        while (std::getline(stream, field, separator)) {
            fields.push_back(field);
        }
        return fields;
    }

    // DRIVER;imie i nazwisko;ladownosc_kg;dostepny (1/0)
    model::Driver parseDriver(const std::vector<std::string>& fields, int lineNumber) const {
        if (fields.size() != 4) {
            throw error(lineNumber, "DRIVER wymaga pol: imie i nazwisko;ladownosc_kg;dostepny");
        }

        model::Driver driver;
        driver.name = fields[1];
        driver.capacityKg = parseNumber(fields[2], lineNumber);
        if (driver.capacityKg <= 0) {
            throw error(lineNumber, "ladownosc musi byc wieksza od 0");
        }
        if (fields[3] == "1") {
            driver.isAvailable = true;
        } else if (fields[3] == "0") {
            driver.isAvailable = false;
        } else {
            throw error(lineNumber, "dostepny musi byc 1 albo 0");
        }
        return driver;
    }

    double parseNumber(const std::string& text, int lineNumber) const {
        try {
            std::size_t used = 0;
            double value = std::stod(text, &used);
            if (used == text.size()) {
                return value;
            }
        } catch (const std::exception&) {
            // obslugiwane ponizej - jeden komunikat dla kazdej zlej liczby
        }
        throw error(lineNumber, "\"" + text + "\" nie jest liczba");
    }

    std::runtime_error error(int lineNumber, const std::string& message) const {
        return std::runtime_error(path_ + ", linia " + std::to_string(lineNumber) + ": " + message);
    }
};

}  // namespace io
