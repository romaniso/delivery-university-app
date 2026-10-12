// Punkt wejscia programu.
// Na razie sprawdza tylko, czy projekt sie buduje i SQLite jest poprawnie podlinkowane.
// Logike aplikacji dopisujemy w modulach w src/ (patrz README.md).

#include <sqlite3.h>

#include <iostream>

int main() {
    std::cout << "Kolejka Paczek - setup OK\n";
    std::cout << "SQLite " << sqlite3_libversion() << '\n';

    // PROJECT_ROOT_DIR ustawia CMake - sciezka do katalogu projektu,
    // przydatna do znalezienia sql/ i data/ niezaleznie od katalogu uruchomienia.
    std::cout << "Katalog projektu: " << PROJECT_ROOT_DIR << '\n';

    // TODO: polaczenie z baza, model, struktury, algorytm przydzialu, symulacja

    return 0;
}
