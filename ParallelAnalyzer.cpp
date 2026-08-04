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

    // construir los resultados de los aeropuertos a partir de los acumuladores
    std::vector<airport::GroupResult> buildAirportResults(
        const std::vector<GroupAccumulator>& accumulators)
    {
        std::vector<airport::GroupResult> results(accumulators.size());

        for (std::size_t index = 0; index < accumulators.size(); ++index)
        {
            const GroupAccumulator& accumulator = accumulators[index];
            airport::GroupResult& result = results[index];

            result.id = static_cast<int>(index);
            result.totalFlights = accumulator.total;
            result.delayedFlights = accumulator.delayed;

            if (accumulator.total > 0)
            {
                result.delayRatePercent =
                    static_cast<double>(accumulator.delayed) * 100.0 /
                    static_cast<double>(accumulator.total);
                result.averageConcurrentFlights =
                    accumulator.concurrentSum /
                    static_cast<double>(accumulator.total);
                result.averagePlaneAge =
                    accumulator.validPlaneAgeCount > 0
                    ? accumulator.planeAgeSum /
                        static_cast<double>(accumulator.validPlaneAgeCount)
                    : 0.0;
                result.averageTemperature =
                    accumulator.temperatureSum /
                    static_cast<double>(accumulator.total);
                result.averageWindSpeed =
                    accumulator.windSum /
                    static_cast<double>(accumulator.total);
            }
        }

        return results;
    }

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
            threadCount = value;
        }

        int ParallelAnalyzer::getThreadCount() const
        {
            return threadCount;
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
            const std::vector<FlightRecord>& records = dataSet.getRecords();
            const int groupCount = dataSet.getMaxAirportId() + 1;

            if (groupCount <= 0)
            {
                return std::vector<GroupResult>();
            }

            std::vector<GroupAccumulator> accumulators(
                static_cast<std::size_t>(groupCount));

            const int actualThreadCount = threadCount > 0
                ? threadCount
                : omp_get_max_threads();

#pragma omp parallel num_threads(actualThreadCount)
            {
                std::vector<GroupAccumulator> localAccumulators(
                    static_cast<std::size_t>(groupCount));

#pragma omp for nowait
                for (int index = 0; index < static_cast<int>(records.size()); ++index)
                {
                    const FlightRecord& record = records[static_cast<std::size_t>(index)];

                    assert(record.departingAirportId >= 0);
                    assert(record.departingAirportId < groupCount);

                    GroupAccumulator& accumulator = localAccumulators[
                        static_cast<std::size_t>(record.departingAirportId)];

                    accumulator.total++;
                    accumulator.delayed += record.delayedOver15Minutes;
                    accumulator.concurrentSum += record.concurrentFlights;
                    accumulator.planeAgeSum += record.planeAge;
                    if (record.planeAge >= 0)
                    {
                        accumulator.validPlaneAgeCount++;
                    }
                    accumulator.temperatureSum += record.maximumTemperature;
                    accumulator.windSum += record.averageWindSpeed;
                }

#pragma omp critical
                {
                    for (int groupIndex = 0; groupIndex < groupCount; ++groupIndex)
                    {
                        GroupAccumulator& destination = accumulators[
                            static_cast<std::size_t>(groupIndex)];
                        const GroupAccumulator& source = localAccumulators[
                            static_cast<std::size_t>(groupIndex)];

                        destination.total += source.total;
                        destination.delayed += source.delayed;
                        destination.concurrentSum += source.concurrentSum;
                        destination.planeAgeSum += source.planeAgeSum;
                        destination.validPlaneAgeCount += source.validPlaneAgeCount;
                        destination.temperatureSum += source.temperatureSum;
                        destination.windSum += source.windSum;
                    }
                }
            }

            return buildAirportResults(accumulators);
        }
    }

}
