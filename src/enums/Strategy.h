// Strategia kolejnosci paczek przy przydziale (README.md, "Score i strategie").
// Kazdej wartosci odpowiada funktor z assignment/Comparators.h.

#pragma once

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

namespace enums {

enum class Strategy {
    FIFO,      // A - kolejnosc przyjecia (FifoOrder)
    PRIORITY,  // B - URGENT przed NORMAL (PriorityOrder)
    SCORE      // C - pelny score: priorytet + aging + termin (ScoreOrder)
};

inline std::string toString(Strategy strategy) {
    switch (strategy) {
        case Strategy::FIFO: return "A - FIFO";
        case Strategy::PRIORITY: return "B - priorytet";
        case Strategy::SCORE: return "C - pelny score";
    }
    return "?";  // nie powinno sie zdarzyc - kazda wartosc jest obsluzona wyzej
}

// Strategia z argumentu wiersza polecen: litera (A/B/C) albo nazwa (FIFO/PRIORITY/SCORE),
// bez rozrozniania wielkosci liter. Nieznana nazwa -> std::nullopt.
inline std::optional<Strategy> fromString(std::string name) {
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    if (name == "A" || name == "FIFO") return Strategy::FIFO;
    if (name == "B" || name == "PRIORITY") return Strategy::PRIORITY;
    if (name == "C" || name == "SCORE") return Strategy::SCORE;
    return std::nullopt;
}

}
