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

#include "enums/Priority.h"
#include "model/Delivery.h"
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
            } else if (fields[0] == "DELIVERY") {
                warehouse.addDelivery(parseDelivery(fields, lineNumber));
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

    // DELIVERY;adres;waga_kg;NORMAL|URGENT;dzien_przyjecia;termin
    model::Delivery parseDelivery(const std::vector<std::string>& fields, int lineNumber) const {
        if (fields.size() != 6) {
            throw error(lineNumber,
                        "DELIVERY wymaga pol: adres;waga_kg;NORMAL|URGENT;dzien_przyjecia;termin");
        }

        model::Delivery delivery;
        delivery.address = fields[1];
        if (delivery.address.empty()) {
            throw error(lineNumber, "adres nie moze byc pusty");
        }
        delivery.weightKg = parseNumber(fields[2], lineNumber);
        if (delivery.weightKg <= 0) {
            throw error(lineNumber, "waga musi byc wieksza od 0");
        }
        try {
            delivery.priority = enums::priorityFromString(fields[3]);
        } catch (const std::invalid_argument& e) {
            throw error(lineNumber, e.what());
        }
        delivery.arrivalDay = parseDay(fields[4], lineNumber);
        delivery.deadlineDay = parseDay(fields[5], lineNumber);
        if (delivery.deadlineDay < delivery.arrivalDay) {
            throw error(lineNumber, "termin nie moze byc wczesniej niz dzien przyjecia");
        }
        return delivery;
    }

    // Dzien to liczba calkowita >= 1 (dni numerujemy od 1)
    int parseDay(const std::string& text, int lineNumber) const {
        try {
            std::size_t used = 0;
            int value = std::stoi(text, &used);
            if (used == text.size()) {
                if (value < 1) {
                    throw error(lineNumber, "dzien musi byc >= 1, jest " + text);
                }
                return value;
            }
        } catch (const std::invalid_argument&) {
            // obslugiwane ponizej
        } catch (const std::out_of_range&) {
            // obslugiwane ponizej
        }
        throw error(lineNumber, "\"" + text + "\" nie jest numerem dnia (liczba calkowita)");
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
