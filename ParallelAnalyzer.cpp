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
            if (value > 0)
            {
                threadCount = value;
            }
            else
            {
                threadCount = omp_get_max_threads();
            }
        }

        int ParallelAnalyzer::getThreadCount() const
        {
            return threadCount;
        }

        GeneralStatistics ParallelAnalyzer::calculateGeneralStatistics(
            const FlightDataSet& dataSet) const
        {
			return GeneralStatistics();
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByMonth(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(12);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByDayOfWeek(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(7);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByDepartureBlock(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxDepartureBlockId() + 1);
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByCarrier(
            const FlightDataSet& dataSet) const
        {
            // Obtener ID máximo de aerolínea
            int maxCarrier =
                dataSet.getMaxCarrierId();

            // Obtener todos los vuelos
            const std::vector<FlightRecord>& records =
                dataSet.getRecords();

            int recordCount =
                records.size();

            // Determinar cuántos hilos se utilizarán
            int threads;

            if (threadCount > 0)
            {
                threads = threadCount;
            }
            else
            {
                threads = omp_get_max_threads();
            }

            // Crear acumuladores independientes para cada hilo
            // Esto evita race conditions
            std::vector<
                std::vector<GroupAccumulator>
            > localAccumulators(threads);

            for (int i = 0; i < threads; i++)
            {
                localAccumulators[i].resize(
                    maxCarrier + 1);
            }

            // Región paralela
#pragma omp parallel num_threads(threads)
            {
                // Identificar hilo actual
                int threadId =
                    omp_get_thread_num();

                // Obtener acumulador privado del hilo
                std::vector<GroupAccumulator>& local =
                    localAccumulators[threadId];

                // Repartir los vuelos entre los hilos
#pragma omp for schedule(static)
                for (int i = 0;
                    i < recordCount;
                    i++)
                {
                    int carrierId =
                        records[i].carrierId;

                    // Ignorar IDs inválidos
                    if (carrierId < 0 ||
                        carrierId > maxCarrier)
                    {
                        continue;
                    }

                    // Contar vuelo
                    local[carrierId].total++;

                    // Contar retraso
                    if (records[i].delayedOver15Minutes != 0)
                    {
                        local[carrierId].delayed++;
                    }
                }
            }

            // Acumulador global donde se unirán los resultados de todos los hilos
            std::vector<GroupAccumulator>
                global(maxCarrier + 1);

            // Reducción manual de resultados
            for (int thread = 0;
                thread < threads;
                thread++)
            {
                for (int carrierId = 0;
                    carrierId <= maxCarrier;
                    carrierId++)
                {
                    global[carrierId].total +=
                        localAccumulators[thread]
                        [carrierId]
                        .total;

                    global[carrierId].delayed +=
                        localAccumulators[thread]
                        [carrierId]
                        .delayed;
                }
            }

            const int minimumVolume = 30;

            std::vector<GroupResult> results;

            // Construir resultados finales
            for (int carrierId = 0;
                carrierId <= maxCarrier;
                carrierId++)
            {
                if (global[carrierId].total == 0)
                {
                    continue;
                }

                GroupResult result;

                // Guardar ID
                result.id = carrierId;

                // Crear nombre de aerolínea
                result.name =
                    "Aerolinea " +
                    std::to_string(carrierId);

                // Advertencia de bajo volumen
                if (global[carrierId].total <
                    minimumVolume)
                {
                    result.name +=
                        " (poco volumen)";
                }

                // Guardar estadísticas básicas
                result.totalFlights =
                    global[carrierId].total;

                result.delayedFlights =
                    global[carrierId].delayed;

                // Calcular porcentaje de retrasos
                result.delayRatePercent =
                    (global[carrierId].delayed * 100.0)
                    /
                    global[carrierId].total;

                results.push_back(result);
            }

            // Crear ranking ordenado por tasa de retraso
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

        std::vector<GroupResult> ParallelAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxAirportId() + 1);
        }

        FactorAnalysis ParallelAnalyzer::calculateFactorAnalysis(
            const FlightDataSet& dataSet) const
        {
			return FactorAnalysis();
        }
    }

}
