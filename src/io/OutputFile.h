// Zapis raportu do pliku wyjsciowego (patrz README.md, "Plik wyjsciowy").

#pragma once

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

#include "enums/DeliveryStatus.h"
#include "enums/Priority.h"
#include "model/Delivery.h"
#include "model/Driver.h"
#include "model/DriverDay.h"
#include "storage/Warehouse.h"

namespace io {

class OutputFile {
public:
    explicit OutputFile(const std::string& path) : path_(path) {}

    // Kierowcy trafiaja do raportu w podanej kolejnosci - sortuje wywolujacy.
    // Kursy - w kolejnosci dni, tak jak zwraca je assignment::DeliveryManager::deliver().
    void write(const storage::Warehouse& warehouse, const std::vector<model::Driver>& drivers,
               const std::vector<model::DriverDay>& driverDays) const {
        std::ofstream file(path_);
        if (!file) {
            throw std::runtime_error("Nie mozna zapisac pliku wyjsciowego: " + path_);
        }
        file << std::fixed << std::setprecision(1);

        file << "Kolejka Paczek - raport\n\n";
        writeDrivers(file, drivers);
        writeDeliveries(file, warehouse.deliveries());
        writeDriverDays(file, warehouse, driverDays);
        writeLeftInWarehouse(file, warehouse.deliveries());
    }

private:
    std::string path_;

    static void writeDrivers(std::ofstream& file, const std::vector<model::Driver>& drivers) {
        file << "Kierowcy (wg ladownosci):\n";
        file << std::left << std::setw(4) << "id" << std::setw(24) << "imie i nazwisko"
             << std::right << std::setw(12) << "ladownosc" << "  dostepny\n";
        for (const model::Driver& driver : drivers) {
            file << std::left << std::setw(4) << driver.id << std::setw(24) << driver.name
                 << std::right << std::setw(9) << driver.capacityKg << " kg" << "  "
                 << (driver.isAvailable ? "tak" : "nie") << '\n';
        }
    }

    static void writeDeliveries(std::ofstream& file,
                                const std::vector<model::Delivery>& deliveries) {
        file << "\nPaczki:\n";
        file << std::left << std::setw(4) << "id" << std::setw(32) << "adres" << std::right
             << std::setw(12) << "waga" << "  " << std::left << std::setw(10) << "priorytet"
             << std::right << std::setw(9) << "przyjeta" << std::setw(8) << "termin" << "  "
             << std::left << std::setw(11) << "status" << std::right << std::setw(9)
             << "kierowca" << std::setw(10) << "doreczona" << '\n';
        for (const model::Delivery& delivery : deliveries) {
            file << std::left << std::setw(4) << delivery.id << std::setw(32) << delivery.address
                 << std::right << std::setw(9) << delivery.weightKg << " kg" << "  " << std::left
                 << std::setw(10) << enums::toString(delivery.priority) << std::right
                 << std::setw(9) << delivery.arrivalDay << std::setw(8) << delivery.deadlineDay
                 << "  " << std::left << std::setw(11) << enums::toString(delivery.status)
                 << std::right << std::setw(9) << orDash(delivery.driverId) << std::setw(10)
                 << orDash(delivery.deliveredDay) << '\n';
        }
    }

    // Lista adresow kazdego kierowcy w kolejnosci doreczania, dzien po dniu
    static void writeDriverDays(std::ofstream& file, const storage::Warehouse& warehouse,
                                const std::vector<model::DriverDay>& driverDays) {
        file << "\nKursy:\n";
        if (driverDays.empty()) {
            file << "  brak - zadna paczka nie zostala przydzielona\n";
            return;
        }
        int currentDay = 0;
        for (const model::DriverDay& driverDay : driverDays) {
            if (driverDay.day != currentDay) {
                currentDay = driverDay.day;
                file << "Dzien " << currentDay << ":\n";
            }
            const model::Driver& driver = warehouse.driver(driverDay.driverId);
            file << "  " << driver.name << " (id " << driver.id << ") - " << driverDay.loadKg << " / "
                 << driver.capacityKg << " kg\n";
            int position = 1;
            for (int id : driverDay.deliveryIds) {
                const model::Delivery& delivery = warehouse.delivery(id);
                file << "    " << position++ << ". " << delivery.address << " (paczka " << id
                     << ", " << delivery.weightKg << " kg, "
                     << enums::toString(delivery.priority) << ")\n";
            }
        }
    }

    static void writeLeftInWarehouse(std::ofstream& file,
                                     const std::vector<model::Delivery>& deliveries) {
        file << "\nZostaly w magazynie:\n";
        bool any = false;
        for (const model::Delivery& delivery : deliveries) {
            if (delivery.status == enums::DeliveryStatus::WAITING ||
                delivery.status == enums::DeliveryStatus::POSTPONED) {
                file << "  paczka " << delivery.id << ": " << delivery.address << ", "
                     << delivery.weightKg << " kg, " << enums::toString(delivery.priority)
                     << ", termin " << delivery.deadlineDay << '\n';
                any = true;
            }
        }
        if (!any) {
            file << "  brak\n";
        }
    }

    // 0 w Delivery oznacza "jeszcze nie ustawione" - w raporcie pokazujemy "-"
    static std::string orDash(int value) { return value == 0 ? "-" : std::to_string(value); }
};

}  // namespace io
