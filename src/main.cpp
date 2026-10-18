// Punkt wejscia programu - spina moduly razem (patrz README.md).
// Uzycie: kolejka [plik_wejsciowy] [plik_wyjsciowy]

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "assignment/Comparators.h"
#include "io/InputFile.h"
#include "io/OutputFile.h"
#include "storage/Warehouse.h"

int main(int argc, char* argv[]) {
    if (argc > 3) {
        std::cerr << "Uzycie: kolejka [plik_wejsciowy] [plik_wyjsciowy]\n";
        return 1;
    }

    // PROJECT_ROOT_DIR ustawia CMake - sciezka do katalogu projektu,
    // dzieki niej domyslne pliki dzialaja niezaleznie od katalogu uruchomienia.
    const std::string root = PROJECT_ROOT_DIR;
    const std::string inputPath = argc > 1 ? std::string(argv[1]) : root + "/data/input.txt";
    const std::string outputPath = argc > 2 ? std::string(argv[2]) : root + "/output/result.txt";

    std::cout << "Kolejka Paczek\n";

    try {
        storage::Warehouse warehouse;
        io::InputFile(inputPath).loadInto(warehouse);
        std::cout << "Wczytano z: " << inputPath << '\n';
        std::cout << "Kierowcy: " << warehouse.drivers().size() << '\n';

        std::vector<model::Driver> drivers = warehouse.drivers();
        std::sort(drivers.begin(), drivers.end(), assignment::DriverCapacityGreater());

        // TODO: przydzial paczek (assignment), symulacja

        io::OutputFile(outputPath).write(drivers);
        std::cout << "Zapisano wynik do: " << outputPath << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Blad: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
