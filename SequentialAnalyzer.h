#pragma once

#include <vector>

#include "FlightDataSet.h"
#include "Models.h"

namespace airport
{
    namespace sequential
    {
        class SequentialAnalyzer
        {
        public:

            SequentialAnalyzer();
            ~SequentialAnalyzer();

            GeneralStatistics calculateGeneralStatistics(
                const FlightDataSet& dataSet) const;

            std::vector<GroupResult> calculateByMonth(
                const FlightDataSet& dataSet) const;

            std::vector<GroupResult> calculateByDayOfWeek(
                const FlightDataSet& dataSet) const;

            std::vector<GroupResult> calculateByDepartureBlock(
                const FlightDataSet& dataSet) const;

            std::vector<GroupResult> calculateByCarrier(
                const FlightDataSet& dataSet) const;

            std::vector<GroupResult> calculateByAirport(
                const FlightDataSet& dataSet) const;

            FactorAnalysis calculateFactorAnalysis(
                const FlightDataSet& dataSet) const;
        };
    }
}



