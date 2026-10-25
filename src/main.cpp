// Punkt wejscia programu - spina moduly razem (patrz README.md).
// Uzycie: kolejka [plik_wejsciowy] [plik_wyjsciowy]

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "assignment/Comparators.h"
#include "enums/DeliveryStatus.h"
#include "enums/Strategy.h"
#include "io/InputFile.h"
#include "io/OutputFile.h"
#include "simulation/Simulation.h"
#include "storage/Warehouse.h"
#include "statistics/Statistics.h"

// Strategia przydzialu. TODO: wybor strategii przez uzytkownika (CLI)
const enums::Strategy STRATEGY = enums::Strategy::SCORE;

int main(int argc, char* argv[]) {
    if (argc > 3) {
        std::cerr << "Uzycie: kolejka [plik_wejsciowy] [plik_wyjsciowy]\n";
        return 1;
    }

    // PROJECT_ROOT_DIR ustawia CMake - sciezka do katalogu projektu,
    // dzieki niej domyslne pliki dzialaja niezaleznie od katalogu uruchomienia.
    const std::string root = PROJECT_ROOT_DIR;
    // @TODO: should be as user input to point out to the source file from data folder, so a user can dynamically choose one
    const std::string inputPath = argc > 1 ? std::string(argv[1]) : root + "/data/input.txt";
    const std::string outputPath = argc > 2 ? std::string(argv[2]) : root + "/output/result.txt";

    std::cout << "Kolejka Paczek\n";

    try {
        storage::Warehouse warehouse;
        io::InputFile(inputPath).loadInto(warehouse);
        std::cout << "Wczytano z: " << inputPath << '\n';
        std::cout << "Kierowcy: " << warehouse.drivers().size() << '\n';
        std::cout << "Paczki: " << warehouse.deliveries().size() << '\n';

        simulation::Simulation simulation(warehouse, STRATEGY);

        const double maxCapacityKg = simulation.maxAvailableCapacity();
        for (const model::Delivery& delivery : warehouse.deliveries()) {
            if (maxCapacityKg > 0 && delivery.weightKg > maxCapacityKg) {
                std::cout << "Uwaga: paczka " << delivery.id << " (" << delivery.weightKg
                          << " kg) nie zmiesci sie u zadnego dostepnego kierowcy (max "
                          << maxCapacityKg << " kg)\n";
            }
        }
        if (maxCapacityKg == 0 && !warehouse.deliveries().empty()) {
            std::cout << "Uwaga: brak dostepnych kierowcow - zadna paczka nie zostanie przydzielona\n";
        }

        std::cout << "Strategia: " << enums::toString(simulation.strategy()) << '\n';
        simulation.run();
        const statistics::Report report = statistics::collect(
            warehouse, simulation.driverDays(), simulation.days());
        const std::string metricsPath = root + "/output/metrics.csv";
        std::ofstream metricsFile(metricsPath);
        if (!metricsFile) {
            throw std::runtime_error("Nie mozna zapisac pliku z metrykami: " + metricsPath);
        }
        statistics::writeCsvHeader(metricsFile);
        statistics::writeCsvRow(metricsFile, simulation.strategy(), report);
        std::cout << "Metryki zapisano do: " << metricsPath << '\n';

        std::cout << "Dni symulacji: " << simulation.days() << '\n';
        std::cout << "Doreczone: " << report.delivered << " / " << report.totalDeliveries << '\n';

        std::vector<model::Driver> drivers = warehouse.drivers();
        std::sort(drivers.begin(), drivers.end(), assignment::DriverCapacityGreater());

        io::OutputFile(outputPath).write(warehouse, drivers, simulation.driverDays(),
                                         simulation.strategy(), simulation.days());
        std::cout << "Zapisano wynik do: " << outputPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Blad: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
