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
        double distanceSum;

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
            distanceSum(0.0),
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
            std::vector<GroupResult> result(12);
            std::vector<GroupAccumulator> accum(12);

            std::string monthName[12] =
            {
                "January", "February", "March", "April",
                "May", "June", "July", "August",
                "September", "October", "November", "December"
            };

            for (int i = 0; i < 12; i++)
            {
                result[i].name = monthName[i];
            }

            int n = dataSet.getRecordCount();
            const std::vector<FlightRecord>& records = dataSet.getRecords();
            
            for (int i = 0; i < n; i++)
            {
                const FlightRecord& record = records[i];
                GroupAccumulator& month = accum[record.month - 1];

                month.total++;
                if (record.delayedOver15Minutes) month.delayed++;
                month.concurrentSum += record.concurrentFlights;
                if (record.planeAge>=0 && record.planeAge <= 100)
                {
                    month.planeAgeSum += record.planeAge;
                    month.validPlaneAgeCount++;
                }
                month.temperatureSum += record.maximumTemperature;
                month.windSum += record.averageWindSpeed;
            }

            for (int i = 0; i < 12; i++)
            {
                result[i].id = i + 1;
                result[i].totalFlights = accum[i].total;
                result[i].delayedFlights = accum[i].delayed;
                if (accum[i].total > 0)
                {
                    result[i].delayRatePercent = 100.0 * accum[i].delayed / accum[i].total ;
                    result[i].averageConcurrentFlights = accum[i].concurrentSum / accum[i].total;
                    if (accum[i].validPlaneAgeCount>0) result[i].averagePlaneAge = accum[i].planeAgeSum / accum[i].validPlaneAgeCount;
                    result[i].averageTemperature = accum[i].temperatureSum / accum[i].total;
                    result[i].averageWindSpeed = accum[i].windSum / accum[i].total;
                }
            }
			return result;
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByDayOfWeek(
            const FlightDataSet& dataSet) const
        {
            std::vector<GroupResult> result(7);
            std::vector<GroupAccumulator> accum(12);

			std::string dayName[7] =
			{
				"Monday", "Tuesday", "Wednesday", "Thursday",
				"Friday", "Saturday", "Sunday"
			};

            for (int i = 0; i < 7; i++)
            {
                result[i].name = dayName[i];
            }

			int n = dataSet.getRecordCount();
            const std::vector<FlightRecord>& records = dataSet.getRecords();

            for (int i = 0; i < n; i++)
            {
                const FlightRecord& record = records[i];
				GroupAccumulator& day = accum[record.dayOfWeek - 1];

                day.total++;
                if (record.delayedOver15Minutes) day.delayed++;
                day.concurrentSum += record.concurrentFlights;

                if (record.planeAge >= 0 && record.planeAge <= 100)
                {
                    day.planeAgeSum += record.planeAge;
                    day.validPlaneAgeCount++;
                }
                day.temperatureSum += record.maximumTemperature;
                day.windSum += record.averageWindSpeed;
            }

            for (int i = 0; i < 7; i++)
            {
                result[i].id = i + 1;
                result[i].totalFlights = accum[i].total;
                result[i].delayedFlights = accum[i].delayed;
                if (accum[i].total > 0)
                {
                    result[i].delayRatePercent = 100.0 * accum[i].delayed / accum[i].total;
                    result[i].averageConcurrentFlights = accum[i].concurrentSum / accum[i].total;
                    if (accum[i].validPlaneAgeCount > 0) result[i].averagePlaneAge = accum[i].planeAgeSum / accum[i].validPlaneAgeCount;
                    result[i].averageTemperature = accum[i].temperatureSum / accum[i].total;
                    result[i].averageWindSpeed = accum[i].windSum / accum[i].total;
                }
            }

			return result;
        }

        std::vector<GroupResult>
            SequentialAnalyzer::calculateByDepartureBlock(
                const FlightDataSet& dataSet) const
        {
            const std::vector<FlightRecord>& records =
                dataSet.getRecords();

            if (records.empty())
            {
                return std::vector<GroupResult>();
            }

            int maxBlockId =
                dataSet.getMaxDepartureBlockId();

            std::vector<GroupAccumulator> accumulators(
                maxBlockId + 1);

            for (int i = 0; i < (int)records.size(); i++)
            {
                const FlightRecord& record = records[i];

                int blockId =
                    record.departureTimeBlockId;

                if (blockId < 0 || blockId > maxBlockId)
                {
                    continue;
                }

                GroupAccumulator& accumulator =
                    accumulators[blockId];

                accumulator.total++;

                if (record.delayedOver15Minutes == 1)
                {
                    accumulator.delayed++;
                }

                accumulator.concurrentSum +=
                    record.concurrentFlights;

                accumulator.temperatureSum +=
                    record.maximumTemperature;

                accumulator.windSum +=
                    record.averageWindSpeed;

                if (record.planeAge >= 0)
                {
                    accumulator.planeAgeSum +=
                        record.planeAge;

                    accumulator.validPlaneAgeCount++;
                }
            }

            std::vector<GroupResult> results;

            for (int blockId = 0;
                blockId <= maxBlockId;
                blockId++)
            {
                const GroupAccumulator& accumulator =
                    accumulators[blockId];

                if (accumulator.total == 0)
                {
                    continue;
                }

                GroupResult result;

                result.id = blockId;
                result.name = "";

                result.totalFlights =
                    accumulator.total;

                result.delayedFlights =
                    accumulator.delayed;

                result.delayRatePercent =
                    ((double)accumulator.delayed * 100.0) /
                    (double)accumulator.total;

                result.averageConcurrentFlights =
                    accumulator.concurrentSum /
                    (double)accumulator.total;

                if (accumulator.validPlaneAgeCount > 0)
                {
                    result.averagePlaneAge =
                        accumulator.planeAgeSum /
                        (double)accumulator.validPlaneAgeCount;
                }
                else
                {
                    result.averagePlaneAge = 0.0;
                }

                result.averageTemperature =
                    accumulator.temperatureSum /
                    (double)accumulator.total;

                result.averageWindSpeed =
                    accumulator.windSum /
                    (double)accumulator.total;

                results.push_back(result);
            }

            return results;
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByCarrier(
            const FlightDataSet& dataSet) const
        {
            // Obtener el ID más grande de aerolínea presente en el conjunto de datos
            int maxCarrier = dataSet.getMaxCarrierId();

            // Crear un acumulador para cada aerolínea
            // La posición del vector coincide con el ID de la aerolínea
            std::vector<GroupAccumulator> accumulators(maxCarrier + 1);

            // Obtener todos los registros de vuelos
            const std::vector<FlightRecord>& records =
                dataSet.getRecords();

            // Recorrer todos los vuelos uno por uno
            for (int i = 0; i < records.size(); i++)
            {
                int carrierId = records[i].carrierId;

                // Ignorar IDs inválidos
                if (carrierId < 0 || carrierId > maxCarrier)
                {
                    continue;
                }

                // Contar vuelo para la aerolínea correspondiente
                accumulators[carrierId].total++;

                // Contar retrasos mayores a 15 minutos
                if (records[i].delayedOver15Minutes != 0)
                {
                    accumulators[carrierId].delayed++;
                }
            }

            // Umbral mínimo para considerar representativa la muestra
            const int minimumVolume = 30;

            std::vector<GroupResult> results;

            // Construir resultado final para cada aerolínea
            for (int carrierId = 0;
                carrierId <= maxCarrier;
                carrierId++)
            {
                // Ignorar aerolíneas sin vuelos
                if (accumulators[carrierId].total == 0)
                {
                    continue;
                }

                GroupResult result;

                // Guardar ID
                result.id = carrierId;

                // Crear nombre básico usando el ID
                result.name =
                    "Aerolinea " +
                    std::to_string(carrierId);

                // Advertir cuando existen pocos vuelos
                if (accumulators[carrierId].total <
                    minimumVolume)
                {
                    result.name +=
                        " (poco volumen)";
                }

                // Guardar estadísticas básicas
                result.totalFlights =
                    accumulators[carrierId].total;

                result.delayedFlights =
                    accumulators[carrierId].delayed;

                // Calcular porcentaje de retrasos
                result.delayRatePercent =
                    (accumulators[carrierId].delayed * 100.0)
                    /
                    accumulators[carrierId].total;

                results.push_back(result);
            }

            // Ordenar de mayor a menor tasa de retraso
            std::sort(
                results.begin(),
                results.end(),
                [](const GroupResult& a,
                    const GroupResult& b)
                {
                    return a.delayRatePercent >
                        b.delayRatePercent;
                });

            // Regresar ranking final
            return results;
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
            const std::vector<FlightRecord>& records = dataSet.getRecords();
            const int maxAirportId = dataSet.getMaxAirportId();

            std::vector<GroupResult> result(maxAirportId + 1);

            if (maxAirportId < 0)
            {
                return std::vector<GroupResult>();
            }

            std::vector<GroupAccumulator> accumulators(maxAirportId + 1);

            for (int i = 0; i < static_cast<int>(records.size()); ++i)
            {
                const FlightRecord& record = records[static_cast<std::size_t>(i)];
                const int airportId = record.departingAirportId;

                if (airportId < 0 || airportId > maxAirportId)
                {
                    continue;
                }

                GroupAccumulator& accumulator = accumulators[airportId];

                accumulator.total++;

                if (record.delayedOver15Minutes == 1)
                {
                    accumulator.delayed++;
                }

                accumulator.concurrentSum += record.concurrentFlights;
                accumulator.planeAgeSum += record.planeAge;
                accumulator.temperatureSum += record.maximumTemperature;
                accumulator.windSum += record.averageWindSpeed;

                if (record.planeAge >= 0)
                {
                    accumulator.validPlaneAgeCount++;
                }
            }

            for (int airportId = 0; airportId <= maxAirportId; ++airportId)
            {
                const GroupAccumulator& accumulator = accumulators[airportId];

                if (accumulator.total == 0)
                {
                    continue;
                }

                result[airportId].id = airportId;
                result[airportId].name = "";
                result[airportId].totalFlights = accumulator.total;
                result[airportId].delayedFlights = accumulator.delayed;
                result[airportId].delayRatePercent =
                    (static_cast<double>(accumulator.delayed) * 100.0) /
                    static_cast<double>(accumulator.total);
                result[airportId].averageConcurrentFlights =
                    accumulator.concurrentSum /
                    static_cast<double>(accumulator.total);

                if (accumulator.validPlaneAgeCount > 0)
                {
                    result[airportId].averagePlaneAge =
                        accumulator.planeAgeSum /
                        static_cast<double>(accumulator.validPlaneAgeCount);
                }
                else
                {
                    result[airportId].averagePlaneAge = 0.0;
                }

                result[airportId].averageTemperature =
                    accumulator.temperatureSum /
                    static_cast<double>(accumulator.total);
                result[airportId].averageWindSpeed =
                    accumulator.windSum /
                    static_cast<double>(accumulator.total);
            }

            return result;
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