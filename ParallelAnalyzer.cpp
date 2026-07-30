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
        std::vector<GroupResult>
            ParallelAnalyzer::calculateByDepartureBlock(
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

            int maxBlockId =
                dataSet.getMaxDepartureBlockId();

            /*
             * Si el usuario configuró una cantidad de hilos,
             * se utiliza esa cantidad.
             *
             * Si threadCount vale cero, se utiliza el máximo
             * disponible de OpenMP.
             */
            int numberOfThreads;

            if (threadCount > 0)
            {
                numberOfThreads = threadCount;
            }
            else
            {
                numberOfThreads =
                    omp_get_max_threads();
            }

            if (numberOfThreads < 1)
            {
                numberOfThreads = 1;
            }

            /*
             * Se crea un vector de acumuladores para cada hilo.
             *
             * Cada hilo modifica únicamente su propio vector,
             * evitando condiciones de carrera.
             */
            std::vector<std::vector<GroupAccumulator>>
                localAccumulators(
                    numberOfThreads,
                    std::vector<GroupAccumulator>(
                        maxBlockId + 1));

#pragma omp parallel num_threads(numberOfThreads)
            {
                int threadId =
                    omp_get_thread_num();

                std::vector<GroupAccumulator>& local =
                    localAccumulators[threadId];

                /*
                 * OpenMP reparte los vuelos entre los hilos.
                 */
#pragma omp for schedule(static)
                for (int i = 0;
                    i < (int)records.size();
                    i++)
                {
                    const FlightRecord& record =
                        records[i];

                    int blockId =
                        record.departureTimeBlockId;

                    /*
                     * Se descartan IDs que no estén
                     * dentro del rango permitido.
                     */
                    if (blockId < 0 ||
                        blockId > maxBlockId)
                    {
                        continue;
                    }

                    GroupAccumulator& accumulator =
                        local[blockId];

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
            }

            /*
             * Se crean los acumuladores finales.
             */
            std::vector<GroupAccumulator> accumulators(
                maxBlockId + 1);

            /*
             * Se combinan los resultados parciales
             * de todos los hilos.
             */
            for (int threadId = 0;
                threadId < numberOfThreads;
                threadId++)
            {
                for (int blockId = 0;
                    blockId <= maxBlockId;
                    blockId++)
                {
                    const GroupAccumulator& source =
                        localAccumulators
                        [threadId]
                        [blockId];

                    GroupAccumulator& destination =
                        accumulators[blockId];

                    destination.total +=
                        source.total;

                    destination.delayed +=
                        source.delayed;

                    destination.concurrentSum +=
                        source.concurrentSum;

                    destination.planeAgeSum +=
                        source.planeAgeSum;

                    destination.validPlaneAgeCount +=
                        source.validPlaneAgeCount;

                    destination.temperatureSum +=
                        source.temperatureSum;

                    destination.windSum +=
                        source.windSum;
                }
            }

            std::vector<GroupResult> results;

            /*
             * Se calculan los resultados finales
             * para cada bloque horario.
             */
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

                /*
                 * El nombre será asignado posteriormente
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
