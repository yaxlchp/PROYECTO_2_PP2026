#pragma once

#include <vector>

#include "FlightDataSet.h"
#include "Models.h"

namespace airport
{
    namespace parallel
    {
        class ParallelAnalyzer
        {
        public:
            explicit ParallelAnalyzer(int threadCount = 0);
            ~ParallelAnalyzer();

            void setThreadCount(int value);
            int getThreadCount() const;

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

        protected:
            int threadCount;
        };
    }
}



