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
        std::vector<GroupResult>
            SequentialAnalyzer::calculateByDepartureBlock(
                const FlightDataSet& dataSet) const
        {
            const std::vector<FlightRecord>& records =
                dataSet.getRecords();

            /*
             * Si no existen vuelos, se regresa
             * un vector vacío.
             */
            if (records.empty())
            {
                return std::vector<GroupResult>();
            }

            /*
             * Se obtiene el ID más grande de los bloques
             * horarios para crear los acumuladores.
             */
            int maxBlockId =
                dataSet.getMaxDepartureBlockId();

            std::vector<GroupAccumulator> accumulators(
                maxBlockId + 1);

            /*
             * Se recorren todos los vuelos.
             */
            for (int i = 0;
                i < (int)records.size();
                i++)
            {
                const FlightRecord& record =
                    records[i];

                int blockId =
                    record.departureTimeBlockId;

                /*
                 * Se evita utilizar un ID inválido
                 * como posición del vector.
                 */
                if (blockId < 0 ||
                    blockId > maxBlockId)
                {
                    continue;
                }

                GroupAccumulator& accumulator =
                    accumulators[blockId];

                /*
                 * Se cuenta el vuelo dentro de su
                 * bloque horario.
                 */
                accumulator.total++;

                /*
                 * Se cuenta como retrasado cuando el campo
                 * delayedOver15Minutes vale uno.
                 */
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

                /*
                 * Solamente se consideran edades válidas
                 * para calcular el promedio.
                 */
                if (record.planeAge >= 0)
                {
                    accumulator.planeAgeSum +=
                        record.planeAge;

                    accumulator.validPlaneAgeCount++;
                }
            }

            std::vector<GroupResult> results;

            /*
             * Se recorren los acumuladores para generar
             * los resultados finales.
             */
            for (int blockId = 0;
                blockId <= maxBlockId;
                blockId++)
            {
                const GroupAccumulator& accumulator =
                    accumulators[blockId];

                /*
                 * No se agrega un bloque que no tenga vuelos.
                 */
                if (accumulator.total == 0)
                {
                    continue;
                }

                GroupResult result;

                /*
                 * Se conserva el ID correspondiente al
                 * bloque horario.
                 */
                result.id = blockId;

                /*
                 * El nombre real debe obtenerse después
                 * mediante DictionaryCatalog.
                 */
                result.name = "";

                result.totalFlights =
                    accumulator.total;

                result.delayedFlights =
                    accumulator.delayed;

                /*
                 * Porcentaje de vuelos retrasados.
                 */
                result.delayRatePercent =
                    ((double)accumulator.delayed * 100.0) /
                    (double)accumulator.total;

                /*
                 * Promedio de vuelos concurrentes.
                 */
                result.averageConcurrentFlights =
                    accumulator.concurrentSum /
                    (double)accumulator.total;

                /*
                 * Promedio de edad de los aviones.
                 */
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

                /*
                 * Promedio de temperatura.
                 */
                result.averageTemperature =
                    accumulator.temperatureSum /
                    (double)accumulator.total;

                /*
                 * Promedio de velocidad del viento.
                 */
                result.averageWindSpeed =
                    accumulator.windSum /
                    (double)accumulator.total;

                results.push_back(result);
            }

            return results;
        }
    }
}