// Symulacja dzien po dniu (README.md, "Zdarzenia i cykl dnia"):
//   DRIVER_AVAILABLE -> NEW_DELIVERY -> assign() -> DEPART -> DELIVERED.
// Jedna symulacja = jedna strategia. Zeby porownac strategie, uruchamiamy kilka symulacji,
// kazda na wlasnej kopii magazynu (storage::Warehouse da sie skopiowac).

#pragma once

#include <vector>

#include "assignment/Comparators.h"
#include "assignment/DeliveryManager.h"
#include "enums/Strategy.h"
#include "model/Delivery.h"
#include "model/DriverDay.h"
#include "storage/Warehouse.h"

namespace simulation {

// Ile dni po ostatniej dostawie symulacja jeszcze probuje przydzielic paczki,
// zanim sie podda (ochrona przed paczkami, ktore nigdy sie nie zmieszcza)
const int EXTRA_DAYS = 30;

class Simulation {
public:
    // Zmienia paczki w podanym magazynie (statusy, kierowcy, dni doreczenia)
    Simulation(storage::Warehouse& warehouse, enums::Strategy strategy)
        : warehouse_(warehouse), strategy_(strategy), deliveryManager_(warehouse) {}

    // Petla po dniach. Konczy sie, gdy po ostatnim dniu przyjecia magazyn jest pusty,
    // albo po ostatnim dniu przyjecia + EXTRA_DAYS.
    void run() {
        int lastArrivalDay = 0;
        for (const model::Delivery& delivery : warehouse_.deliveries()) {
            if (delivery.arrivalDay > lastArrivalDay) {
                lastArrivalDay = delivery.arrivalDay;
            }
        }

        for (int day = 1; day <= lastArrivalDay + EXTRA_DAYS; ++day) {
            deliveryManager_.driversAvailable();
            assign(day);
            deliveryManager_.depart();
            for (const model::DriverDay& driverDay : deliveryManager_.deliver(day)) {
                driverDays_.push_back(driverDay);
            }
            days_ = day;
            if (day >= lastArrivalDay && !deliveryManager_.hasWaitingDeliveries()) {
                break;
            }
        }
    }

    enums::Strategy strategy() const { return strategy_; }

    // Ile dni trwala symulacja (0 = jeszcze nie uruchomiona)
    int days() const { return days_; }

    // Kursy kierowcow dzien po dniu - do raportu (i metryk)
    const std::vector<model::DriverDay>& driverDays() const { return driverDays_; }

    // Najwieksza ladownosc wsrod dostepnych kierowcow - do ostrzezen przed symulacja
    double maxAvailableCapacity() const { return deliveryManager_.maxAvailableCapacity(); }

private:
    storage::Warehouse& warehouse_;
    enums::Strategy strategy_;
    assignment::DeliveryManager deliveryManager_;
    std::vector<model::DriverDay> driverDays_;
    int days_ = 0;

    // Wybor funktora dla strategii. Kazdy tworzymy na dany dzien,
    // bo ScoreOrder liczy score wzgledem dnia.
    void assign(int day) {
        switch (strategy_) {
            case enums::Strategy::FIFO:
                deliveryManager_.assign(day, assignment::FifoOrder{warehouse_});
                break;
            case enums::Strategy::PRIORITY:
                deliveryManager_.assign(day, assignment::PriorityOrder{warehouse_});
                break;
            case enums::Strategy::SCORE:
                deliveryManager_.assign(day, assignment::ScoreOrder{warehouse_, day});
                break;
        }
    }
};

}  // namespace simulation
