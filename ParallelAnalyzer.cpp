#include "ParallelAnalyzer.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <omp.h>
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
    namespace parallel
    {
        ParallelAnalyzer::ParallelAnalyzer(int threadCount)
        {
            if (threadCount > 0)
            {
                this->threadCount = threadCount;
            }
            else
            {
                this->threadCount = omp_get_max_threads();
            }
        }

        ParallelAnalyzer::~ParallelAnalyzer()
        {

        }

        void ParallelAnalyzer::setThreadCount(int value)
        {
            if (value > 0)
            {
                threadCount = value;
            }
            else
            {
                threadCount = omp_get_max_threads();
            }
        }

        int ParallelAnalyzer::getThreadCount() const
        {
            return threadCount;
        }

        GeneralStatistics ParallelAnalyzer::calculateGeneralStatistics(
            const FlightDataSet& dataSet) const
        {
			return GeneralStatistics();
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByMonth(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(12);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByDayOfWeek(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(7);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByDepartureBlock(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxDepartureBlockId() + 1);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByCarrier(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxCarrierId() + 1);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxAirportId() + 1);
        }

        FactorAnalysis ParallelAnalyzer::calculateFactorAnalysis(
            const FlightDataSet& dataSet) const
        {
			return FactorAnalysis();
        }
    }

}
