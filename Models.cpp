#include "Models.h"

namespace airport
{
    FlightRecord::FlightRecord()
        : month(0),
        dayOfWeek(0),
        delayedOver15Minutes(0),
        departureTimeBlockId(0),
        distanceGroup(0),
        concurrentFlights(0),
        numberOfSeats(0),
        carrierId(0),
        airportFlightsMonth(0),
        airlineFlightsMonth(0),
        planeAge(0),
        departingAirportId(0),
        precipitation(0.0f),
        snow(0.0f),
        snowDepth(0.0f),
        maximumTemperature(0.0f),
        averageWindSpeed(0.0f)
    {
    }

    GeneralStatistics::GeneralStatistics()
        : totalFlights(0),
        delayedFlights(0),
        delayRatePercent(0.0),
        averageConcurrentFlights(0.0),
        averageSeats(0.0),
        averagePlaneAge(0.0),
        averagePrecipitation(0.0),
        averageSnow(0.0),
        averageSnowDepth(0.0),
        averageTemperature(0.0),
        averageWindSpeed(0.0),
        minimumPlaneAge(-1),
        maximumPlaneAge(-1)
    {
    }

    GroupResult:: GroupResult()
        : id(0),
        name(""),
        totalFlights(0),
        delayedFlights(0),
        delayRatePercent(0.0),
        averageConcurrentFlights(0.0),
        averagePlaneAge(0.0),
        averageTemperature(0.0),
        averageWindSpeed(0.0)
    {
    }

    BinaryComparison::BinaryComparison()
        : factorName(""),
        delayedCount(0),
        onTimeCount(0),
        delayedAverage(0.0),
        onTimeAverage(0.0),
        difference(0.0)
    {
    }

    BenchmarkResult::BenchmarkResult()
        : sequentialSeconds(0.0),
        parallelSeconds(0.0),
        speedup(0.0),
        efficiencyPercent(0.0),
        threads(1)
    {
    }
}
