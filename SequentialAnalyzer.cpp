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
            // Obtener el ID máximo de aerolínea
            int maxCarrier = dataSet.getMaxCarrierId();

            // Guardar datos temporales de cada aerolínea
            std::vector<GroupAccumulator> accumulators(maxCarrier + 1);

            // Obtener todos los vuelos
            const std::vector<FlightRecord>& records = dataSet.getRecords();

            // Revisar todos los vuelos
            for (int i = 0; i < records.size(); i++)
            {
                // Obtener ID de la aerolínea
                int carrierId = records[i].carrierId;

                // Ignorar datos incorrectos
                if (carrierId < 0 || carrierId > maxCarrier)
                {
                    continue;
                }

                // Contar vuelos
                accumulators[carrierId].total++;

                // Contar retrasos
                if (records[i].delayedOver15Minutes != 0)
                {
                    accumulators[carrierId].delayed++;
                }

                // Sumar datos para promedios
                accumulators[carrierId].concurrentSum +=
                    records[i].concurrentFlights;


                accumulators[carrierId].temperatureSum +=
                    records[i].maximumTemperature;


                accumulators[carrierId].windSum +=
                    records[i].averageWindSpeed;


                // Guardar edad del avión
                if (records[i].planeAge >= 0)
                {
                    accumulators[carrierId].planeAgeSum +=
                        records[i].planeAge;


                    accumulators[carrierId].validPlaneAgeCount++;
                }
            }

            int minimumVolume = 30;

            // Vector final donde la posición coincide con el ID
            std::vector<GroupResult> results(maxCarrier + 1);

            // Crear resultados
            for (int carrierId = 0; carrierId <= maxCarrier; carrierId++)
            {
                GroupAccumulator accumulator = accumulators[carrierId];


                GroupResult result;



                // Guardar ID
                result.id = carrierId;

                // Crear nombre
                if (accumulator.total == 0)
                {
                    result.name = "";
                }
                else
                {
                    result.name =
                        "Aerolinea " + std::to_string(carrierId);



                    if (accumulator.total < minimumVolume)
                    {
                        result.name += " (poco volumen)";
                    }
                }


                // Guardar cantidades
                result.totalFlights = accumulator.total;

                result.delayedFlights = accumulator.delayed;


                // Calcular valores
                if (accumulator.total > 0)
                {
                    result.delayRatePercent =
                        (accumulator.delayed * 100.0) /
                        accumulator.total;


                    result.averageConcurrentFlights =
                        accumulator.concurrentSum /
                        accumulator.total;


                    result.averageTemperature =
                        accumulator.temperatureSum /
                        accumulator.total;


                    result.averageWindSpeed =
                        accumulator.windSum /
                        accumulator.total;
                }

                else
                {
                    result.delayRatePercent = 0;

                    result.averageConcurrentFlights = 0;

                    result.averageTemperature = 0;

                    result.averageWindSpeed = 0;
                }

                // Promedio de edad
                if (accumulator.validPlaneAgeCount > 0)
                {
                    result.averagePlaneAge =
                        accumulator.planeAgeSum /
                        accumulator.validPlaneAgeCount;
                }
                else
                {
                    result.averagePlaneAge = 0;
                }

                // Guardar usando el ID como posición
                results[carrierId] = result;
            }

            //Crear ranking de retrasos

            std::vector<GroupResult> ranking = results;

            // Ordenar de mayor a menor tasa de retraso
            for (int i = 0; i < ranking.size(); i++)
            {
                for (int j = i + 1; j < ranking.size(); j++)
                {
                    if (ranking[j].delayRatePercent >
                        ranking[i].delayRatePercent)
                    {
                        GroupResult temporary = ranking[i];


                        ranking[i] = ranking[j];


                        ranking[j] = temporary;
                    }
                }
            }

            // Aquí ranking contiene el orden de mayor retraso
            // Ejemplo:
            // ranking[0] = aerolínea con más retrasos
            // ranking[1] = segunda con más retrasos
            return results;
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
