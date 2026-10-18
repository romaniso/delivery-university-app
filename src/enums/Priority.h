// Priorytet paczki - URGENT trafia do kierowcy przed NORMAL.
// Tekst ("NORMAL" / "URGENT") jest taki sam w pliku wejsciowym i w raporcie.

#pragma once

#include <stdexcept>
#include <string>

namespace enums {

enum class Priority {
    NORMAL,
    URGENT
};

inline std::string toString(Priority priority) {
    switch (priority) {
        case Priority::NORMAL: return "NORMAL";
        case Priority::URGENT: return "URGENT";
    }
    throw std::logic_error("nieznana wartosc Priority");
}

// Rzuca std::invalid_argument - io::InputFile dokleja do komunikatu numer linii
inline Priority priorityFromString(const std::string& text) {
    if (text == "NORMAL") {
        return Priority::NORMAL;
    }
    if (text == "URGENT") {
        return Priority::URGENT;
    }
    throw std::invalid_argument("\"" + text + "\" nie jest priorytetem (NORMAL/URGENT)");
}

}
