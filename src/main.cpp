// Punkt wejscia programu - spina moduly razem (patrz README.md).
// Uzycie: kolejka [-s A|B|C] [plik_wejsciowy] [plik_wyjsciowy]

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "assignment/Comparators.h"
#include "enums/DeliveryStatus.h"
#include "enums/Strategy.h"
#include "io/InputFile.h"
#include "io/OutputFile.h"
#include "io/Prompt.h"
#include "simulation/Simulation.h"
#include "storage/Warehouse.h"
#include "statistics/Statistics.h"

// Wartosci domyslne pytan po uruchomieniu (Enter albo brak danych na wejsciu)
const enums::Strategy DEFAULT_STRATEGY = enums::Strategy::SCORE;
const std::string DEFAULT_INPUT_NAME = "input.txt";    // w data/
const std::string DEFAULT_OUTPUT_NAME = "result.txt";  // w output/

const char* const USAGE =
    "Uzycie: kolejka [-s A|B|C] [plik_wejsciowy] [plik_wyjsciowy]\n"
    "  -s, --strategy  A (FIFO), B (PRIORITY) lub C (SCORE)\n"
    "O wszystko, czego nie podano w argumentach, program pyta po uruchomieniu.\n";

int main(int argc, char* argv[]) {
    std::optional<enums::Strategy> cliStrategy;
    std::vector<std::string> paths;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-s" || arg == "--strategy") {
            if (i + 1 >= argc) {
                std::cerr << "Brak nazwy strategii po " << arg << '\n' << USAGE;
                return 1;
            }
            const std::optional<enums::Strategy> parsed = enums::fromString(argv[++i]);
            if (!parsed) {
                std::cerr << "Nieznana strategia: " << argv[i] << '\n' << USAGE;
                return 1;
            }
            cliStrategy = parsed;
        } else {
            paths.push_back(arg);
        }
    }
    if (paths.size() > 2) {
        std::cerr << USAGE;
        return 1;
    }

    // PROJECT_ROOT_DIR ustawia CMake - sciezka do katalogu projektu,
    // dzieki niej data/ i output/ dzialaja niezaleznie od katalogu uruchomienia.
    const std::string root = PROJECT_ROOT_DIR;

    std::cout << "Kolejka Paczek\n";
    const std::string inputPath =
        paths.size() > 0 ? paths[0] : io::askInputPath(root + "/data", DEFAULT_INPUT_NAME);
    const enums::Strategy strategy =
        cliStrategy ? *cliStrategy : io::askStrategy(DEFAULT_STRATEGY);
    const std::string outputPath =
        paths.size() > 1 ? paths[1] : io::askOutputPath(root + "/output", DEFAULT_OUTPUT_NAME);

    try {
        storage::Warehouse warehouse;
        io::InputFile(inputPath).loadInto(warehouse);
        std::cout << "Wczytano z: " << inputPath << '\n';
        std::cout << "Kierowcy: " << warehouse.drivers().size() << '\n';
        std::cout << "Paczki: " << warehouse.deliveries().size() << '\n';

        simulation::Simulation simulation(warehouse, strategy);

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
