// Menedzer doreczen - przydziela paczki z magazynu kierowcom i prowadzi zdarzenia dnia
// (README.md, "Przydzial - assign()" i "Zdarzenia i cykl dnia"):
//   DRIVER_AVAILABLE -> NEW_DELIVERY -> assign() -> DEPART -> DELIVERED.
// Magazyn tylko przechowuje dane - cala logika przydzialu jest tutaj.

#pragma once

#include <queue>
#include <vector>

#include "enums/DeliveryStatus.h"
#include "enums/QueueStatus.h"
#include "model/Delivery.h"
#include "model/Driver.h"
#include "model/DriverQueue.h"
#include "model/DriverDay.h"
#include "storage/Warehouse.h"

namespace assignment {

class DeliveryManager {
public:
    // Jedna kolejka na kierowce, w kolejnosci id (kolejka kierowcy n lezy pod indeksem n - 1)
    explicit DeliveryManager(storage::Warehouse& warehouse) : warehouse_(warehouse) {
        for (const model::Driver& driver : warehouse_.drivers()) {
            model::DriverQueue queue;
            queue.driverId = driver.id;
            queues_.push_back(queue);
        }
    }

    // DRIVER_AVAILABLE: kierowcy, ktorzy wczoraj wyjechali, wracaja - kolejka znow OPEN i pusta
    void driversAvailable() {
        for (model::DriverQueue& queue : queues_) {
            if (queue.status == enums::QueueStatus::DEPARTED) {
                queue.status = enums::QueueStatus::OPEN;
                queue.loadKg = 0.0;
            }
        }
    }

    // NEW_DELIVERY nie wymaga osobnej metody: paczki sa WAITING od wczytania pliku,
    // a assign() bierze tylko te z arrivalDay <= dzis - w dniu przyjecia staja sie widoczne.

    // Order - strategia z assignment/Comparators.h (np. ScoreOrder).
    // Paczki WAITING i POSTPONED, ktore juz przyszly, trafiaja do kolejki priorytetowej
    // budowanej od nowa. Zdejmowane od gory: jest kierowca -> ASSIGNED, brak miejsca -> POSTPONED.
    // Status zmieniamy dopiero po pop(), wiec nie psuje porzadku paczek, ktore sa jeszcze w kolejce.
    template <typename Order>
    void assign(int today, Order order) {
        std::priority_queue<int, std::vector<int>, Order> waiting(order);
        for (const model::Delivery& delivery : warehouse_.deliveries()) {
            if (isInWarehouse(delivery) && delivery.arrivalDay <= today) {
                waiting.push(delivery.id);
            }
        }

        while (!waiting.empty()) {
            model::Delivery& delivery = warehouse_.delivery(waiting.top());
            waiting.pop();

            int driverId = findBestFit(delivery.weightKg);
            if (driverId == 0) {
                // lzejsze paczki nizej w kolejce nadal moga sie zmiescic - nie przerywamy petli
                delivery.status = enums::DeliveryStatus::POSTPONED;
                continue;
            }
            model::DriverQueue& queue = queues_[driverId - 1];
            queue.deliveries.push(delivery.id);
            queue.loadKg += delivery.weightKg;
            delivery.status = enums::DeliveryStatus::ASSIGNED;
            delivery.driverId = queue.driverId;
        }
    }

    // DEPART: kazda niepusta kolejka sie zamyka, jej paczki sa w drodze
    void depart() {
        for (model::DriverQueue& queue : queues_) {
            if (queue.status != enums::QueueStatus::OPEN || queue.deliveries.empty()) {
                continue;
            }
            queue.status = enums::QueueStatus::DEPARTED;
            // std::queue nie ma iteratorow - przechodzimy po kopii
            std::queue<int> inTruck = queue.deliveries;
            while (!inTruck.empty()) {
                warehouse_.delivery(inTruck.front()).status = enums::DeliveryStatus::IN_TRANSIT;
                inTruck.pop();
            }
        }
    }

    // DELIVERED: kierowcy, ktorzy wyjechali, doreczaja paczki od poczatku kolejki.
    // Zwraca kursy dnia - kolejnosc doreczania do raportu.
    std::vector<model::DriverDay> deliver(int today) {
        std::vector<model::DriverDay> driverDays;
        for (model::DriverQueue& queue : queues_) {
            if (queue.status != enums::QueueStatus::DEPARTED || queue.deliveries.empty()) {
                continue;
            }
            model::DriverDay driverDay;
            driverDay.driverId = queue.driverId;
            driverDay.day = today;
            driverDay.loadKg = queue.loadKg;
            while (!queue.deliveries.empty()) {
                model::Delivery& delivery = warehouse_.delivery(queue.deliveries.front());
                queue.deliveries.pop();
                delivery.status = enums::DeliveryStatus::DELIVERED;
                delivery.deliveredDay = today;
                driverDay.deliveryIds.push_back(delivery.id);
            }
            driverDays.push_back(driverDay);
        }
        return driverDays;
    }

    // Czy w magazynie zostaly paczki (WAITING lub POSTPONED)
    bool hasWaitingDeliveries() const {
        for (const model::Delivery& delivery : warehouse_.deliveries()) {
            if (isInWarehouse(delivery)) {
                return true;
            }
        }
        return false;
    }

    // Najwieksza ladownosc wsrod dostepnych kierowcow (0 = nikt nie jest dostepny).
    // Paczka ciezsza niz ta wartosc nigdy nie zostanie przydzielona.
    double maxAvailableCapacity() const {
        double maxKg = 0.0;
        for (const model::Driver& driver : warehouse_.drivers()) {
            if (driver.isAvailable && driver.capacityKg > maxKg) {
                maxKg = driver.capacityKg;
            }
        }
        return maxKg;
    }

private:
    storage::Warehouse& warehouse_;
    std::vector<model::DriverQueue> queues_;

    static bool isInWarehouse(const model::Delivery& delivery) {
        return delivery.status == enums::DeliveryStatus::WAITING ||
               delivery.status == enums::DeliveryStatus::POSTPONED;
    }

    // Best fit: sposrod dostepnych kierowcow z otwarta kolejka i wolnym miejscem
    // wybiera tego, ktoremu po dolozeniu paczki zostanie najmniej miejsca.
    // Remis: mniejsze id (kolejki sa w kolejnosci id).
    // Zwraca id kierowcy, 0 = nikt nie pasuje.
    int findBestFit(double weightKg) const {
        int bestDriverId = 0;
        double bestFreeKg = 0.0;
        for (const model::DriverQueue& queue : queues_) {
            const model::Driver& driver = warehouse_.driver(queue.driverId);
            if (!driver.isAvailable || queue.status != enums::QueueStatus::OPEN) {
                continue;
            }
            double freeKg = driver.capacityKg - queue.loadKg - weightKg;
            if (freeKg < 0) {
                continue;
            }
            if (bestDriverId == 0 || freeKg < bestFreeKg) {
                bestDriverId = queue.driverId;
                bestFreeKg = freeKg;
            }
        }
        return bestDriverId;
    }
};

}  // namespace assignment
