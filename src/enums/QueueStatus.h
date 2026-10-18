// Status kolejki kierowcy: OPEN - mozna dokladac paczki,
// DEPARTED - kierowca wyjechal, kolejka zamknieta (zdarzenie DEPART).

#pragma once

#include <stdexcept>
#include <string>

namespace enums {

enum class QueueStatus {
    OPEN,
    DEPARTED
};

inline std::string toString(QueueStatus status) {
    switch (status) {
        case QueueStatus::OPEN: return "OPEN";
        case QueueStatus::DEPARTED: return "DEPARTED";
    }
    throw std::logic_error("nieznana wartosc QueueStatus");
}

}
