// Pytania do uzytkownika po uruchomieniu: strategia, plik wejsciowy, plik wyjsciowy.
// Kazde pytanie ma wartosc domyslna (Enter). Koniec wejscia (np. uruchomienie bez
// terminala) tez daje wartosc domyslna, wiec program nigdy nie czeka w nieskonczonosc.

#pragma once

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "enums/Strategy.h"

namespace io {

// Wypisuje pytanie i czyta jedna linie bez bialych znakow na brzegach.
// Zwraca false na koncu wejscia.
inline bool readAnswer(const std::string& question, std::string& answer) {
    std::cout << question;
    if (!std::getline(std::cin, answer)) {
        std::cout << '\n';
        return false;
    }
    answer.erase(0, answer.find_first_not_of(" \t\r"));
    answer.erase(answer.find_last_not_of(" \t\r") + 1);
    return true;
}

// Dopisuje ".txt", gdy nazwa pliku nie ma zadnego rozszerzenia ("wynik" -> "wynik.txt").
inline std::string withTxtExtension(const std::string& path) {
    return std::filesystem::path(path).has_extension() ? path : path + ".txt";
}

// Menu strategii. Przyjmuje numer (1-3), litere (A/B/C) lub nazwe (FIFO/PRIORITY/SCORE).
inline enums::Strategy askStrategy(enums::Strategy defaultStrategy) {
    const enums::Strategy options[] = {enums::Strategy::FIFO, enums::Strategy::PRIORITY,
                                       enums::Strategy::SCORE};
    std::cout << "Wybierz strategie:\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "  " << i + 1 << ") " << enums::toString(options[i]) << '\n';
    }

    std::string answer;
    while (readAnswer("Wybor [1-3, Enter = " + enums::toString(defaultStrategy) + "]: ", answer)) {
        if (answer.empty()) break;
        if (answer == "1" || answer == "2" || answer == "3") return options[answer[0] - '1'];
        if (const std::optional<enums::Strategy> parsed = enums::fromString(answer)) return *parsed;
        std::cout << "Nieznana strategia: " << answer << '\n';
    }
    return defaultStrategy;
}

// Menu pliku wejsciowego: lista plikow .txt z katalogu dataDir (czytana przy kazdym
// uruchomieniu, wiec nowe pliki pojawiaja sie same). Przyjmuje numer z listy, nazwe pliku
// z dataDir albo dowolna sciezke do istniejacego pliku; ".txt" mozna pominac.
inline std::string askInputPath(const std::string& dataDir, const std::string& defaultName) {
    namespace fs = std::filesystem;

    std::vector<std::string> names;
    std::error_code error;
    for (const fs::directory_entry& entry : fs::directory_iterator(dataDir, error)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            names.push_back(entry.path().filename().string());
        }
    }
    std::sort(names.begin(), names.end());

    std::cout << "Wybierz plik wejsciowy (" << dataDir << "):\n";
    for (std::size_t i = 0; i < names.size(); ++i) {
        std::cout << "  " << i + 1 << ") " << names[i] << '\n';
    }

    std::string answer;
    while (readAnswer("Wybor [numer, nazwa lub sciezka, Enter = " + defaultName + "]: ", answer)) {
        if (answer.empty()) break;

        const bool isNumber = std::all_of(answer.begin(), answer.end(),
                                          [](unsigned char c) { return std::isdigit(c); });
        if (isNumber) {
            const std::size_t number = std::stoul(answer);
            if (number >= 1 && number <= names.size()) return dataDir + "/" + names[number - 1];
            std::cout << "Brak pliku o numerze " << answer << '\n';
            continue;
        }

        const std::string candidates[] = {dataDir + "/" + answer,
                                          dataDir + "/" + withTxtExtension(answer), answer,
                                          withTxtExtension(answer)};
        for (const std::string& candidate : candidates) {
            if (fs::is_regular_file(candidate, error)) return candidate;
        }
        std::cout << "Nie znaleziono pliku: " << answer << '\n';
    }
    return dataDir + "/" + defaultName;
}

// Pytanie o plik wyjsciowy. Sama nazwa (bez katalogu) trafia do outputDir,
// sciezka z katalogiem jest uzywana bez zmian. Bez rozszerzenia dopisuje ".txt".
inline std::string askOutputPath(const std::string& outputDir, const std::string& defaultName) {
    std::string answer;
    if (!readAnswer("Plik wyjsciowy [nazwa w " + outputDir + " lub sciezka, Enter = " +
                        defaultName + "]: ",
                    answer) ||
        answer.empty()) {
        return outputDir + "/" + defaultName;
    }
    answer = withTxtExtension(answer);
    if (std::filesystem::path(answer).has_parent_path()) return answer;
    return outputDir + "/" + answer;
}

}
