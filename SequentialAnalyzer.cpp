#include "SequentialAnalyzer.h"

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

        double precipitationSum;
        double temperatureSum;
        double windSum;
        double distanceSum;

        GroupAccumulator()
            : total(0),
            delayed(0),
            concurrentSum(0.0),
            planeAgeSum(0.0),
            validPlaneAgeCount(0),
            precipitationSum(0.0),
            temperatureSum(0.0),
            windSum(0.0),
            distanceSum(0.0)
        {
        }
    };

    airport::BinaryComparison createComparison(
        const char* name,
        double delayedSum,
        long long delayedCount,
        double onTimeSum,
        long long onTimeCount)
    {
        airport::BinaryComparison comparison;

        comparison.factorName = name;

        comparison.delayedCount = delayedCount;
        comparison.onTimeCount = onTimeCount;

        if (delayedCount > 0)
        {
            comparison.delayedAverage =
                delayedSum / delayedCount;
        }

        if (onTimeCount > 0)
        {
            comparison.onTimeAverage =
                onTimeSum / onTimeCount;
        }

        comparison.difference =
            comparison.delayedAverage -
            comparison.onTimeAverage;

        return comparison;
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

        std::vector<GroupResult>
            SequentialAnalyzer::calculateByDepartureBlock(
                const FlightDataSet& dataSet) const
        {
            return std::vector<GroupResult>(
                dataSet.getMaxDepartureBlockId() + 1);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByCarrier(
            const FlightDataSet& dataSet) const
        {
            return std::vector<GroupResult>(
                dataSet.getMaxCarrierId() + 1);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
            return std::vector<GroupResult>(
                dataSet.getMaxAirportId() + 1);
        }

        FactorAnalysis SequentialAnalyzer::calculateFactorAnalysis(
            const FlightDataSet& dataSet) const
        {
            if (dataSet.isEmpty())
            {
                throw exceptions::EmptyDataSetException();
            }

            GroupAccumulator delayed;
            GroupAccumulator onTime;

            const std::vector<FlightRecord>& flights =
                dataSet.getRecords();

            for (const FlightRecord& flight : flights)
            {
                if (flight.delayedOver15Minutes == 1)
                {
                    delayed.total++;
                    delayed.delayed++;

                    delayed.concurrentSum +=
                        flight.concurrentFlights;

                    delayed.precipitationSum +=
                        flight.precipitation;

                    delayed.temperatureSum +=
                        flight.maximumTemperature;

                    delayed.windSum +=
                        flight.averageWindSpeed;

                    delayed.distanceSum +=
                        flight.distanceGroup;

                    if (flight.planeAge >= 0)
                    {
                        delayed.planeAgeSum +=
                            flight.planeAge;

                        delayed.validPlaneAgeCount++;
                    }
                }
                else
                {
                    onTime.total++;

                    onTime.concurrentSum +=
                        flight.concurrentFlights;

                    onTime.precipitationSum +=
                        flight.precipitation;

                    onTime.temperatureSum +=
                        flight.maximumTemperature;

                    onTime.windSum +=
                        flight.averageWindSpeed;

                    onTime.distanceSum +=
                        flight.distanceGroup;

                    if (flight.planeAge >= 0)
                    {
                        onTime.planeAgeSum +=
                            flight.planeAge;

                        onTime.validPlaneAgeCount++;
                    }
                }
            }

            FactorAnalysis result;

            result.concurrentFlights =
                createComparison(
                    "Vuelos concurrentes",
                    delayed.concurrentSum,
                    delayed.total,
                    onTime.concurrentSum,
                    onTime.total);

            result.planeAge =
                createComparison(
                    "Edad del avion",
                    delayed.planeAgeSum,
                    delayed.validPlaneAgeCount,
                    onTime.planeAgeSum,
                    onTime.validPlaneAgeCount);

            result.precipitation =
                createComparison(
                    "Precipitacion",
                    delayed.precipitationSum,
                    delayed.total,
                    onTime.precipitationSum,
                    onTime.total);

            result.windSpeed =
                createComparison(
                    "Velocidad del viento",
                    delayed.windSum,
                    delayed.total,
                    onTime.windSum,
                    onTime.total);

            result.temperature =
                createComparison(
                    "Temperatura",
                    delayed.temperatureSum,
                    delayed.total,
                    onTime.temperatureSum,
                    onTime.total);

            result.distanceGroup =
                createComparison(
                    "Grupo de distancia",
                    delayed.distanceSum,
                    delayed.total,
                    onTime.distanceSum,
                    onTime.total);

            return result;
        }
    }
}