#include "SequentialAnalyzer.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "Exceptions.h"

namespace
{
    struct GroupAccumulator
    {
        long long total;
        long long delayed;
        double concurrentSum;
        double planeAgeSum;
        long long validPlaneAgeCount;
        double temperatureSum;
        double windSum;

        GroupAccumulator()
            : total(0),
            delayed(0),
            concurrentSum(0.0),
            planeAgeSum(0.0),
            validPlaneAgeCount(0),
            temperatureSum(0.0),
            windSum(0.0)
        {
        }
    };

}


namespace airport
{
    namespace sequential
    {
        SequentialAnalyzer::SequentialAnalyzer()
        {
        }

        SequentialAnalyzer::~SequentialAnalyzer()
        {
        }

        GeneralStatistics SequentialAnalyzer::calculateGeneralStatistics(
            const FlightDataSet& dataSet) const
        {
			return GeneralStatistics();
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByMonth(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(12);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByDayOfWeek(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(7);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByDepartureBlock(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxDepartureBlockId() + 1);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByCarrier(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxCarrierId() + 1);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxAirportId() + 1);
        }


        FactorAnalysis SequentialAnalyzer::calculateFactorAnalysis(
            const FlightDataSet& dataSet) const
        {
			return FactorAnalysis();
        }

    }
}
