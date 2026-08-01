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
            // Obtener la cantidad máxima de aerolíneas que existen
            int maxCarrier = dataSet.getMaxCarrierId();

            // Obtener todos los vuelos que vamos a analizar
            const std::vector<FlightRecord>& records = dataSet.getRecords();

            // Guardar la cantidad total de vuelos
            int recordCount = records.size();

            // Revisar cuántos hilos se van a utilizar
            // Si no se recibe una cantidad, se usa la cantidad máxima disponible
            int threads;

            if (threadCount > 0)
            {
                threads = threadCount;
            }
            else
            {
                threads = omp_get_max_threads();
            }

            // Crear un acumulador para cada hilo
            // Cada hilo tendrá sus propios datos para evitar problemas cuando varios hilos intenten modificar el mismo valor
            std::vector<std::vector<GroupAccumulator>> localAccumulators;

            localAccumulators.resize(threads);

            // Crear espacio para guardar las aerolíneas dentro de cada hilo
            for (int i = 0; i < threads; i++)
            {
                localAccumulators[i].resize(maxCarrier + 1);
            }

            //Aquí empieza la parte paralela.
            //Cada hilo revisa una parte de los vuelos.
            //Cada hilo guarda sus resultados por separado, así no se mezclan los datos entre hilos.
 
#pragma omp parallel num_threads(threads)
            {
                // Saber qué número de hilo está trabajando
                int threadId = omp_get_thread_num();



                // Este hilo trabaja solamente con su propio acumulador
                std::vector<GroupAccumulator>& local =
                    localAccumulators[threadId];

                // OpenMP reparte los vuelos entre los diferentes hilos
#pragma omp for schedule(static)
                for (int i = 0; i < recordCount; i++)
                {
                    // Guardar el vuelo actual
                    const FlightRecord& record = records[i];

                    // Obtener la aerolínea del vuelo
                    int carrierId = record.carrierId;

                    // Si la aerolínea no existe, se ignora este vuelo
                    if (carrierId < 0 || carrierId > maxCarrier)
                    {
                        continue;
                    }

                    // Aumentar el número de vuelos de la aerolínea
                    local[carrierId].total++;

                    // Revisar si el vuelo tuvo retraso
                    if (record.delayedOver15Minutes != 0)
                    {
                        local[carrierId].delayed++;
                    }

                    // Guardar datos que después se usarán para calcular promedios
                    local[carrierId].concurrentSum +=
                        record.concurrentFlights;


                    local[carrierId].temperatureSum +=
                        record.maximumTemperature;


                    local[carrierId].windSum +=
                        record.averageWindSpeed;

                    // Guardar la edad del avión si tiene un valor válido
                    if (record.planeAge >= 0)
                    {
                        local[carrierId].planeAgeSum +=
                            record.planeAge;


                        local[carrierId].validPlaneAgeCount++;
                    }
                }
            }
            
            //En esta parte se juntan los resultados de todos los hilos.
            // Se combinan los hilos en un solo lugar.

            std::vector<GroupAccumulator> global(maxCarrier + 1);

            for (int thread = 0; thread < threads; thread++)
            {
                for (int carrierId = 0; carrierId <= maxCarrier; carrierId++)
                {
                    // Obtener los datos guardados por este hilo
                    GroupAccumulator local =
                        localAccumulators[thread][carrierId];

                    // Sumar los datos al acumulador general
                    global[carrierId].total += local.total;


                    global[carrierId].delayed += local.delayed;


                    global[carrierId].concurrentSum +=
                        local.concurrentSum;


                    global[carrierId].planeAgeSum +=
                        local.planeAgeSum;


                    global[carrierId].validPlaneAgeCount +=
                        local.validPlaneAgeCount;


                    global[carrierId].temperatureSum +=
                        local.temperatureSum;


                    global[carrierId].windSum +=
                        local.windSum;
                }
            }

            // Cantidad mínima de vuelos para considerar que la información es suficiente
            int minimumVolume = 30;

            // Crear resultados finales
            // La posición del vector coincide con el ID de la aerolínea
            std::vector<GroupResult> results(maxCarrier + 1);

            // Crear el resultado de cada aerolínea
            for (int carrierId = 0; carrierId <= maxCarrier; carrierId++)
            {
                // Obtener los datos acumulados de la aerolínea
                GroupAccumulator accumulator = global[carrierId];



                // Crear resultado que se va a guardar
                GroupResult result;

                // Guardar ID de la aerolínea
                result.id = carrierId;

                // Crear nombre de la aerolínea
                if (accumulator.total == 0)
                {
                    // Si no tiene vuelos se deja vacío
                    result.name = "";
                }
                else
                {
                    // Nombre temporal usando el ID
                    result.name =
                        "Aerolinea " + std::to_string(carrierId);



                    // Avisar si tiene pocos vuelos
                    if (accumulator.total < minimumVolume)
                    {
                        result.name += " (poco volumen)";
                    }
                }

                // Guardar cantidad de vuelos
                result.totalFlights = accumulator.total;


                // Guardar cantidad de retrasos
                result.delayedFlights = accumulator.delayed;

                // Calcular porcentaje de retrasos y promedios
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
                    // Si no existen vuelos se colocan valores en cero
                    result.delayRatePercent = 0;


                    result.averageConcurrentFlights = 0;


                    result.averageTemperature = 0;


                    result.averageWindSpeed = 0;
                }

                // Calcular edad promedio de los aviones
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

                // Guardar resultado usando el ID como posición
                results[carrierId] = result;
            }

            // Regresar resultados finales
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
