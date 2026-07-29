#pragma once

#include <string>

namespace airport
{
    struct FlightRecord
    {
        int month;
        int dayOfWeek;
        int delayedOver15Minutes;
        int departureTimeBlockId;
        int distanceGroup;
        int concurrentFlights;
        int numberOfSeats;
        int carrierId;
        int airportFlightsMonth;
        int airlineFlightsMonth;
        int planeAge;
        int departingAirportId;

        float precipitation;
        float snow;
        float snowDepth;
        float maximumTemperature;
        float averageWindSpeed;

        FlightRecord();
    };

    struct GeneralStatistics
    {
        long long totalFlights;
        long long delayedFlights;
        double delayRatePercent;
        double averageConcurrentFlights;
        double averageSeats;
        double averagePlaneAge;
        double averagePrecipitation;
        double averageSnow;
        double averageSnowDepth;
        double averageTemperature;
        double averageWindSpeed;
        int minimumPlaneAge;
        int maximumPlaneAge;

        GeneralStatistics();
    };

    struct GroupResult
    {
        int id;
        std::string name;
        long long totalFlights;
        long long delayedFlights;
        double delayRatePercent;
        double averageConcurrentFlights;
        double averagePlaneAge;
        double averageTemperature;
        double averageWindSpeed;

        GroupResult();
    };

    struct BinaryComparison
    {
        std::string factorName;
        long long delayedCount;
        long long onTimeCount;
        double delayedAverage;
        double onTimeAverage;
        double difference;

        BinaryComparison();
    };

    struct FactorAnalysis
    {
        BinaryComparison concurrentFlights;
        BinaryComparison planeAge;
        BinaryComparison precipitation;
        BinaryComparison windSpeed;
        BinaryComparison temperature;
        BinaryComparison distanceGroup;
    };

    struct BenchmarkResult
    {
        double sequentialSeconds;
        double parallelSeconds;
        double speedup;
        double efficiencyPercent;
        int threads;

        BenchmarkResult();
    };
}


