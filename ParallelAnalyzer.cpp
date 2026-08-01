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
                if (dataSet.isEmpty())
                {
                    throw exceptions::EmptyDataSetException();
                }

                const std::vector<FlightRecord>& records = dataSet.getRecords();
                const long long n = (long long)records.size();

                long long delayedFlights = 0;
                double concurrentSum = 0.0;
                double seatesSum = 0.0;
                double planeAgeSum = 0.0;
                long long validPlaneAgeCount = 0;
                double precipitationSum = 0.0;
                double snowSum = 0.0;
                double snowDepthSum = 0.0;
                double temperatureSum = 0.0;
                double windSum = 0.0;
                int minAge = std::numeric_limits<int>::max();
                int maxAge = std::numeric_limits<int>::min();

    #pragma omp parallel for num_threads(threadCount) schedule(static) reduction(+:delayedFlights, concurrentSum, seatsSum, planeAgeSum, validPlaneAgeCount, precipitationSum, snowSum, snowDepthSum, temperatureSum, windSum) reduction (min:minAge) reduction(max:maxAge)

                for (long long i = 0; i < n; i++)
                {
                    const FlightRecord& r = records[i];

                    if (r.delayedOver15Minutes > 0)
                    {
                        delayedFlights++;
                    }

                    concurrentSum += r.concurrentFlights;
                    seatesSum += r.numberOfSeats;
                    precipitationSum += r.precipitation;
                    snowSum += r.snow;
                    snowDepthSum += r.snowDepth;
                    temperatureSum += r.maximumTemperature;
                    windSum += r.averageWindSpeed;

                    if (r.planeAge != -1)
                    {
                        planeAgeSum += r.planeAge;
                        validPlaneAgeCount++;

                        if (r.planeAge < minAge)
                        {
                            minAge = r.planeAge;
                        }

                        if (r.planeAge > maxAge)
                        {
                            maxAge = r.planeAge;
                        }
                    }
                }

                GeneralStatistics stats;
                stats.totalFlights = n;
                stats.delayedFlights = delayedFlights;
                stats.delayRatePercent = ((double)delayedFlights / (double)n) * 100.0;
                stats.averageConcurrentFlights = concurrentSum / (double)n;
                stats.averageSeats = seatesSum / (double)n;
                stats.averagePrecipitation = precipitationSum / (double)n;
                stats.averageSnow = snowSum / (double)n;
                stats.averageSnowDepth = snowDepthSum / (double)n;
                stats.averageTemperature = temperatureSum / (double)n;
                stats.averageWindSpeed = windSum / (double)n;

                if (validPlaneAgeCount > 0)
                {
                    stats.averagePlaneAge = planeAgeSum / (double)validPlaneAgeCount;
                    stats.minimumPlaneAge = minAge;
                    stats.maximumPlaneAge = maxAge;
                }
                else
                {
                    stats.averagePlaneAge = 0.0;
                    stats.minimumPlaneAge = -1;
                    stats.maximumPlaneAge = -1;
                }

			return stats;
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
