// Strategia kolejnosci paczek przy przydziale (README.md, "Score i strategie").
// Kazdej wartosci odpowiada funktor z assignment/Comparators.h.

#pragma once

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

}
