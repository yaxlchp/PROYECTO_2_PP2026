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
    namespace sequential
    {
        SequentialAnalyzer::SequentialAnalyzer()
        {
        }

        SequentialAnalyzer::~SequentialAnalyzer()
        {
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByAirport(
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

            for (const FlightRecord& record : records)
            {
                assert(record.departingAirportId >= 0);
                assert(record.departingAirportId < groupCount);

                GroupAccumulator& accumulator = accumulators[
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

            return buildAirportResults(accumulators);
        }

    }
}
