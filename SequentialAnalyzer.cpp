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
        double seatsSum;

        double planeAgeSum;
        long long validPlaneAgeCount;

        double precipitationSum;
        double snowSum;
        double snowDepthSum;

        double temperatureSum;
        double windSum;

        int minimumPlaneAge;
        int maximumPlaneAge;

        bool firstValidPlaneAge;

        GroupAccumulator()
            : total(0),
            delayed(0),
            concurrentSum(0.0),
            seatsSum(0.0),
            planeAgeSum(0.0),
            validPlaneAgeCount(0),
            precipitationSum(0.0),
            snowSum(0.0),
            snowDepthSum(0.0),
            temperatureSum(0.0),
            windSum(0.0),
            minimumPlaneAge(0),
            maximumPlaneAge(0),
            firstValidPlaneAge(true)
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
            if (dataSet.isEmpty())
            {
                throw exceptions::EmptyDataSetException();
            }

            const std::vector<FlightRecord>& records =
                dataSet.getRecords();

            GroupAccumulator accumulator;

            for (std::size_t i = 0; i < records.size(); i++)
            {
                const FlightRecord& flight = records[i];

                accumulator.total++;

                if (flight.delayedOver15Minutes != 0)
                {
                    accumulator.delayed++;
                }

                accumulator.concurrentSum += flight.concurrentFlights;
                accumulator.seatsSum += flight.numberOfSeats;
                accumulator.precipitationSum += flight.precipitation;
                accumulator.snowSum += flight.snow;
                accumulator.snowDepthSum += flight.snowDepth;
                accumulator.temperatureSum += flight.maximumTemperature;
                accumulator.windSum += flight.averageWindSpeed;

                if (flight.planeAge >= 0)
                {
                    accumulator.planeAgeSum += flight.planeAge;
                    accumulator.validPlaneAgeCount++;

                    if (accumulator.firstValidPlaneAge)
                    {
                        accumulator.minimumPlaneAge = flight.planeAge;
                        accumulator.maximumPlaneAge = flight.planeAge;
                        accumulator.firstValidPlaneAge = false;
                    }
                    else
                    {
                        if (flight.planeAge < accumulator.minimumPlaneAge)
                        {
                            accumulator.minimumPlaneAge = flight.planeAge;
                        }

                        if (flight.planeAge > accumulator.maximumPlaneAge)
                        {
                            accumulator.maximumPlaneAge = flight.planeAge;
                        }
                    }
                }
            }

            GeneralStatistics statistics;

            statistics.totalFlights = accumulator.total;
            statistics.delayedFlights = accumulator.delayed;

            statistics.delayRatePercent =
                accumulator.delayed * 100.0 / accumulator.total;

            statistics.averageConcurrentFlights =
                accumulator.concurrentSum / accumulator.total;

            statistics.averageSeats =
                accumulator.seatsSum / accumulator.total;

            statistics.averagePrecipitation =
                accumulator.precipitationSum / accumulator.total;

            statistics.averageSnow =
                accumulator.snowSum / accumulator.total;

            statistics.averageSnowDepth =
                accumulator.snowDepthSum / accumulator.total;

            statistics.averageTemperature =
                accumulator.temperatureSum / accumulator.total;

            statistics.averageWindSpeed =
                accumulator.windSum / accumulator.total;

            if (accumulator.validPlaneAgeCount > 0)
            {
                statistics.averagePlaneAge =
                    accumulator.planeAgeSum /
                    accumulator.validPlaneAgeCount;

                statistics.minimumPlaneAge =
                    accumulator.minimumPlaneAge;

                statistics.maximumPlaneAge =
                    accumulator.maximumPlaneAge;
            }
            else
            {
                statistics.averagePlaneAge = 0.0;
                statistics.minimumPlaneAge = 0;
                statistics.maximumPlaneAge = 0;
            }

            return statistics;
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
