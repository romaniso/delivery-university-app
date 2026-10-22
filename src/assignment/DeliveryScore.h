// Score paczki dla strategii C - im wyzszy, tym wazniejsza paczka.
// Wzor i przyklad: README.md, "Score i strategie".

#pragma once

#include <algorithm>

#include "enums/Priority.h"
#include "model/Delivery.h"

namespace assignment {

// Wagi score - do dopracowania przy porownaniu strategii
const int URGENT_POINTS = 100;
const int POINTS_PER_WAITING_DAY = 10;
// Punkty za termin: DEADLINE_POINTS w dniu terminu, o DEADLINE_STEP mniej
// za kazdy dzien zapasu (nie mniej niz 0), o DEADLINE_STEP wiecej za kazdy dzien spoznienia
const int DEADLINE_POINTS = 60;
const int DEADLINE_STEP = 20;

inline int deliveryScore(const model::Delivery& delivery, int today) {
    int priorityPoints = delivery.priority == enums::Priority::URGENT ? URGENT_POINTS : 0;
    int agingPoints = POINTS_PER_WAITING_DAY * (today - delivery.arrivalDay);
    int daysLeft = delivery.deadlineDay - today;
    int deadlinePoints = std::max(0, DEADLINE_POINTS - DEADLINE_STEP * daysLeft);
    return priorityPoints + agingPoints + deadlinePoints;
}

}  // namespace assignment
