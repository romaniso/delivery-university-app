// Statystyki zakonczonej symulacji.
#pragma once

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <string>
#include <vector>

#include "enums/DeliveryStatus.h"
#include "enums/Strategy.h"
#include "model/DriverDay.h"
#include "storage/Warehouse.h"

namespace statistics {

struct Report {
    int totalDeliveries = 0;
    int delivered = 0;
    int onTime = 0;
    int late = 0;
    int remaining = 0;
    double averageWaitingDays = 0.0;
    int maxWaitingDays = 0;
    double executorUtilizationPercent = 0.0;
};

inline Report collect(const storage::Warehouse& warehouse,
                      const std::vector<model::DriverDay>& driverDays,
                      int simulationDays) {
    Report report;
    report.totalDeliveries = static_cast<int>(warehouse.deliveries().size());

    int waitingDaysSum = 0;
    for (const model::Delivery& delivery : warehouse.deliveries()) {
        if (delivery.status == enums::DeliveryStatus::DELIVERED) {
            ++report.delivered;
            const int waitingDays = std::max(0, delivery.deliveredDay - delivery.arrivalDay);
            waitingDaysSum += waitingDays;
            report.maxWaitingDays = std::max(report.maxWaitingDays, waitingDays);
            if (delivery.deliveredDay <= delivery.deadlineDay) {
                ++report.onTime;
            } else {
                ++report.late;
            }
        } else {
            ++report.remaining;
        }
    }

    if (report.delivered > 0) {
        report.averageWaitingDays = static_cast<double>(waitingDaysSum) / report.delivered;
    }

    double usedCapacityKg = 0.0;
    for (const model::DriverDay& driverDay : driverDays) {
        usedCapacityKg += driverDay.loadKg;
    }

    double availableCapacityKg = 0.0;
    for (const model::Driver& driver : warehouse.drivers()) {
        if (driver.isAvailable) {
            availableCapacityKg += driver.capacityKg;
        }
    }
    availableCapacityKg *= std::max(0, simulationDays);
    if (availableCapacityKg > 0.0) {
        report.executorUtilizationPercent = 100.0 * usedCapacityKg / availableCapacityKg;
    }

    return report;
}

inline void writeCsvHeader(std::ostream& output) {
    output << "strategy,total_deliveries,delivered,on_time,late,remaining,"
              "average_waiting_days,max_waiting_days,executor_utilization_percent\n";
}

inline void writeCsvRow(std::ostream& output, enums::Strategy strategy, const Report& report) {
    output << enums::toString(strategy) << ',' << report.totalDeliveries << ',' << report.delivered << ','
           << report.onTime << ',' << report.late << ',' << report.remaining << ','
           << std::fixed << std::setprecision(2) << report.averageWaitingDays << ','
           << report.maxWaitingDays << ',' << report.executorUtilizationPercent << "\n";
}

}  // namespace statistics
