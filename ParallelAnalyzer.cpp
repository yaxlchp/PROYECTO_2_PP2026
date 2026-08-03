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

        double precipitationSum; //Variable agregada para el promedio de precipitacion de vuelos astrason y en timepo.
        double distanceGroupSum; //Varibale para tomar la distancia de vuelo promedio de lo vuelos.

        GroupAccumulator()
            : total(0),
            delayed(0),
            concurrentSum(0.0),
            planeAgeSum(0.0),
            validPlaneAgeCount(0),
            temperatureSum(0.0),
            windSum(0.0),
            precipitationSum(0.0), //Promedio de precipitaciones
            distanceGroupSum(0.0) //Promedio de distancia
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
                if (dataSet.isEmpty())
                {
                    throw exceptions::EmptyDataSetException();
                }

                const std::vector<FlightRecord>& records = dataSet.getRecords();
                const long long n = (long long)records.size();

                long long delayedFlights = 0;
                double concurrentSum = 0.0;
                double seatesSum = 0.0;
                double planeAgeSum = 0.0;
                long long validPlaneAgeCount = 0;
                double precipitationSum = 0.0;
                double snowSum = 0.0;
                double snowDepthSum = 0.0;
                double temperatureSum = 0.0;
                double windSum = 0.0;
                int minAge = std::numeric_limits<int>::max();
                int maxAge = std::numeric_limits<int>::min();

    #pragma omp parallel for num_threads(threadCount) schedule(static) reduction(+:delayedFlights, concurrentSum, seatsSum, planeAgeSum, validPlaneAgeCount, precipitationSum, snowSum, snowDepthSum, temperatureSum, windSum) reduction (min:minAge) reduction(max:maxAge)

                for (long long i = 0; i < n; i++)
                {
                    const FlightRecord& r = records[i];

                    if (r.delayedOver15Minutes > 0)
                    {
                        delayedFlights++;
                    }

                    concurrentSum += r.concurrentFlights;
                    seatesSum += r.numberOfSeats;
                    precipitationSum += r.precipitation;
                    snowSum += r.snow;
                    snowDepthSum += r.snowDepth;
                    temperatureSum += r.maximumTemperature;
                    windSum += r.averageWindSpeed;

                    if (r.planeAge != -1)
                    {
                        planeAgeSum += r.planeAge;
                        validPlaneAgeCount++;

                        if (r.planeAge < minAge)
                        {
                            minAge = r.planeAge;
                        }

                        if (r.planeAge > maxAge)
                        {
                            maxAge = r.planeAge;
                        }
                    }
                }

                GeneralStatistics stats;
                stats.totalFlights = n;
                stats.delayedFlights = delayedFlights;
                stats.delayRatePercent = ((double)delayedFlights / (double)n) * 100.0;
                stats.averageConcurrentFlights = concurrentSum / (double)n;
                stats.averageSeats = seatesSum / (double)n;
                stats.averagePrecipitation = precipitationSum / (double)n;
                stats.averageSnow = snowSum / (double)n;
                stats.averageSnowDepth = snowDepthSum / (double)n;
                stats.averageTemperature = temperatureSum / (double)n;
                stats.averageWindSpeed = windSum / (double)n;

                if (validPlaneAgeCount > 0)
                {
                    stats.averagePlaneAge = planeAgeSum / (double)validPlaneAgeCount;
                    stats.minimumPlaneAge = minAge;
                    stats.maximumPlaneAge = maxAge;
                }
                else
                {
                    stats.averagePlaneAge = 0.0;
                    stats.minimumPlaneAge = -1;
                    stats.maximumPlaneAge = -1;
                }

			return stats;
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByMonth(
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

            omp_set_num_threads(getThreadCount());

            #pragma omp parallel
            {
                std::vector<GroupAccumulator> localAccum(12);
                #pragma omp for
                for (int i = 0; i < n; i++)
                {
                    const FlightRecord& record = records[i];
                    GroupAccumulator& month = localAccum[record.month - 1];

                    month.total++;
                    if (record.delayedOver15Minutes) month.delayed++;
                    month.concurrentSum += record.concurrentFlights;
                    if (record.planeAge >= 0 && record.planeAge <= 100)
                    {
                        month.planeAgeSum += record.planeAge;
                        month.validPlaneAgeCount++;
                    }
                    month.temperatureSum += record.maximumTemperature;
                    month.windSum += record.averageWindSpeed;
                }

                #pragma omp critical
                {
                    for (int i = 0; i < 12; i++)
                    {
                        accum[i].total += localAccum[i].total;
                        accum[i].delayed += localAccum[i].delayed;
                        accum[i].concurrentSum += localAccum[i].concurrentSum;
                        accum[i].planeAgeSum += localAccum[i].planeAgeSum;
                        accum[i].validPlaneAgeCount += localAccum[i].validPlaneAgeCount;
                        accum[i].temperatureSum += localAccum[i].temperatureSum;
                        accum[i].windSum += localAccum[i].windSum;
                    }
                }
            }

            for (int i = 0; i < 12; i++)
            {
                result[i].id = i + 1;
                result[i].totalFlights = accum[i].total;
                result[i].delayedFlights = accum[i].delayed;
                if (accum[i].total > 0)
                {
                    result[i].delayRatePercent = 100.0 * accum[i].delayed / accum[i].total;
                    result[i].averageConcurrentFlights = accum[i].concurrentSum / accum[i].total;
                    if (accum[i].validPlaneAgeCount > 0) 
                        result[i].averagePlaneAge = accum[i].planeAgeSum / accum[i].validPlaneAgeCount;
                    result[i].averageTemperature = accum[i].temperatureSum / accum[i].total;
                    result[i].averageWindSpeed = accum[i].windSum / accum[i].total;
                }
            }
            return result;
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByDayOfWeek(
            const FlightDataSet& dataSet) const
        {
            std::vector<GroupResult> result_parallel(7);
            std::vector<GroupAccumulator> accum_pa(7);

            std::string dayName[7] =
            {
				"Monday", "Tuesday", "Wednesday", "Thursday",
                "Friday", "Saturday","Sunday"
            };


            for (int i = 0; i < 7; i++)
            {
                result_parallel[i].name = dayName[i];
            }

            int n = dataSet.getRecordCount();
            const std::vector<FlightRecord>& records = dataSet.getRecords();

#pragma omp parallel
            {
                std::vector<GroupAccumulator> local_accum(12);
#pragma omp for
                for (int i = 0; i < n; i++)
                {
                    const FlightRecord& record = records[i];
                    GroupAccumulator& day = local_accum[record.month - 1];

                    day.total++;


                    if (record.delayedOver15Minutes)
                    {
                        day.delayed++;
                    }

                    day.concurrentSum += record.concurrentFlights;


                    if (record.planeAge >= 0 && record.planeAge <= 100)
                    {
                        day.planeAgeSum += record.planeAge;
                        day.validPlaneAgeCount++;
                    }
                    day.temperatureSum += record.maximumTemperature;
                    day.windSum += record.averageWindSpeed;
                }

#pragma omp critical
                {
                    for (int i = 0; i < 7; i++)
                    {
                        accum_pa[i].total += local_accum[i].total;
                        accum_pa[i].delayed += local_accum[i].delayed;
                        accum_pa[i].concurrentSum += local_accum[i].concurrentSum;
                        accum_pa[i].planeAgeSum += local_accum[i].planeAgeSum;
                        accum_pa[i].validPlaneAgeCount += local_accum[i].validPlaneAgeCount;
                        accum_pa[i].temperatureSum += local_accum[i].temperatureSum;
                        accum_pa[i].windSum += local_accum[i].windSum;
                    }
                }
            }

            for (int i = 0; i < 7; i++)
            {
                result_parallel[i].id = i + 1;
                result_parallel[i].totalFlights = accum_pa[i].total;
                result_parallel[i].delayedFlights = accum_pa[i].delayed;
                
                result_parallel[i].delayRatePercent = 100.0 * accum_pa[i].delayed / accum_pa[i].total;
                result_parallel[i].averageConcurrentFlights = accum_pa[i].concurrentSum / accum_pa[i].total;
                if (accum_pa[i].validPlaneAgeCount > 0)
                {
                    result_parallel[i].averagePlaneAge = accum_pa[i].planeAgeSum / accum_pa[i].validPlaneAgeCount;
                }
                result_parallel[i].averageTemperature = accum_pa[i].temperatureSum / accum_pa[i].total;
                result_parallel[i].averageWindSpeed = accum_pa[i].windSum / accum_pa[i].total;
     
            }

            return result_parallel;
        }

        std::vector<GroupResult> ParallelAnalyzer::calculateByDepartureBlock(
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

            std::vector<std::vector<GroupAccumulator> >
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

#pragma omp for schedule(static)
                for (int i = 0;
                    i < (int)records.size();
                    i++)
                {
                    const FlightRecord& record =
                        records[i];

                    int blockId =
                        record.departureTimeBlockId;

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

            std::vector<GroupAccumulator> accumulators(
                maxBlockId + 1);

            for (int threadId = 0;
                threadId < numberOfThreads;
                threadId++)
            {
                for (int blockId = 0;
                    blockId <= maxBlockId;
                    blockId++)
                {
                    const GroupAccumulator& source =
                        localAccumulators[threadId][blockId];

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

        FactorAnalysis ParallelAnalyzer::calculateFactorAnalysis(const FlightDataSet& dataSet) const
        {
            //Lo que se plantea en este método es tomar absolutamente todos los datos y comparar aquellos que estan atrasados. 
            // Para eso existen las estructuras de FactorAnalysis y BinaryComparision, pues se revisan cada uno de los factores y se "guarda" cuál de todos los factores es aquel que más atrasos tiene.
       
            //Generamos la estructura que va a devolver la función.
            FactorAnalysis Resultado;

            //Hay que recordadr que ya existe una función que toma los datos de los .csv, por lo que solo hay que preocuparse por ir revisando todos los datos.
            //El método se llama " getRecords() ", que proviene de la clase " FlightDataSet " y trabaja con un arreglo dinámico de estructuras " FlightRecord ".
            //FlightDataSet hace referencia a los archivos .csv, mientras que FlightRecord toma los datos de cada vuelo.

            //
            const std::vector<FlightRecord>& Registros = dataSet.getRecords();
            int TotalDeRegistros = static_cast<int>(Registros.size());

            Resultado.concurrentFlights.factorName = "Vuelos concurridos";
            Resultado.planeAge.factorName = "Edad del avión";
            Resultado.precipitation.factorName = "Precipitación";
            Resultado.windSpeed.factorName = "Velocidad del viento";
            Resultado.temperature.factorName = "Temperatura";
            Resultado.distanceGroup.factorName = "Distacia de vuelo";

            GroupAccumulator Grupo_Atrasado;
            GroupAccumulator Grupo_EnTiempo;

            #pragma omp parallel for shared(Grupo_Atrasado,Grupo_EnTiempo,Registros) /*reduction(+: Grupo_Atrasado.total, Grupo_Atrasado.concurentSum, Grupo_Atrasado.temperatureSum, Grupo_Atrasado.windSum, Grupo_Atrasado.planeAgeSum, \
            Grupo_Atrasado.precipitationSum, Grupo_Atrasado.planeSum, Grupo_EnTiempo.total, Grupo_EnTiempo.concurrentSum, Grupo_EnTiempo.temperatureSum, Grupo_EnTiempo.windSum, \
            Grupo_EnTiempo.planeAgeSum, Grupo_EnTiempo.precipitationSum, Grupo_EnTiempo.planeAgeSum )*/
                for (int i = 0; i < TotalDeRegistros; i++)
                {
                    const FlightRecord& Vuelo = Registros[i];

                    if (Vuelo.delayedOver15Minutes == 1)
                    {
                        #pragma omp critical 
                        {
                            Grupo_Atrasado.total++;
                            Grupo_Atrasado.concurrentSum += Vuelo.concurrentFlights;
                            Grupo_Atrasado.temperatureSum += Vuelo.maximumTemperature;
                            Grupo_Atrasado.windSum += Vuelo.averageWindSpeed;
                            Grupo_Atrasado.planeAgeSum += Vuelo.planeAge;
                            Grupo_Atrasado.precipitationSum += Vuelo.precipitation;
                            Grupo_Atrasado.distanceGroupSum += Vuelo.distanceGroup; 

                            if (Vuelo.planeAge >= 25)
                            {
                                Grupo_Atrasado.planeAgeSum += Vuelo.planeAge;
                            }
                        }
                    }
                    else
                    {
                        #pragma omp critical 
                        {
                            Grupo_EnTiempo.total++;
                            Grupo_EnTiempo.concurrentSum += Vuelo.concurrentFlights;
                            Grupo_EnTiempo.temperatureSum += Vuelo.maximumTemperature;
                            Grupo_EnTiempo.windSum += Vuelo.averageWindSpeed;
                            Grupo_EnTiempo.planeAgeSum += Vuelo.planeAge;
                            Grupo_EnTiempo.precipitationSum += Vuelo.precipitation;
                            Grupo_EnTiempo.distanceGroupSum += Vuelo.distanceGroup;

                            if (Vuelo.planeAge >= 0 && Vuelo.planeAge < 25)
                            {
                                Grupo_EnTiempo.planeAgeSum += Vuelo.planeAge;
                            }
                        }
                    }
                }

            //Se asignan la cantidad de aviones atrasados en todos los atributos de la estructur de FactorAnalysis
            Resultado.concurrentFlights.delayedCount = Resultado.distanceGroup.delayedCount = Resultado.planeAge.delayedCount = Resultado.precipitation.delayedCount =
                Resultado.temperature.delayedCount = Resultado.windSpeed.delayedCount = Grupo_Atrasado.total;

            Resultado.concurrentFlights.onTimeCount = Resultado.distanceGroup.onTimeCount = Resultado.planeAge.onTimeCount = Resultado.precipitation.onTimeCount =
                Resultado.temperature.onTimeCount = Resultado.windSpeed.onTimeCount = Grupo_EnTiempo.total;

           // Se consigen los promedios de todos los vuelos atrasados
            if (Grupo_Atrasado.total > 0)
            {
                Resultado.concurrentFlights.delayedAverage = Grupo_Atrasado.concurrentSum / Resultado.concurrentFlights.delayedCount;
                Resultado.planeAge.delayedAverage = Grupo_Atrasado.planeAgeSum / Resultado.planeAge.delayedCount;
                Resultado.precipitation.delayedAverage = Grupo_Atrasado.precipitationSum / Resultado.precipitation.delayedCount;
                Resultado.windSpeed.delayedAverage = Grupo_Atrasado.windSum / Resultado.windSpeed.delayedCount;
                Resultado.temperature.delayedAverage = Grupo_Atrasado.temperatureSum / Resultado.temperature.delayedCount;
                Resultado.distanceGroup.delayedAverage = Grupo_Atrasado.distanceGroupSum / Resultado.distanceGroup.delayedCount;
            }
            // Se consigen los promedios de todos los vuelos en tiempo
            if (Grupo_EnTiempo.total > 0)
            {
                Resultado.concurrentFlights.onTimeAverage = Grupo_EnTiempo.concurrentSum / Resultado.concurrentFlights.onTimeCount;
                Resultado.planeAge.onTimeAverage = Grupo_EnTiempo.planeAgeSum / Resultado.planeAge.onTimeCount;
                Resultado.precipitation.onTimeAverage = Grupo_EnTiempo.precipitationSum / Resultado.precipitation.onTimeCount;
                Resultado.windSpeed.onTimeAverage = Grupo_EnTiempo.windSum / Resultado.windSpeed.onTimeCount;
                Resultado.temperature.onTimeAverage = Grupo_EnTiempo.temperatureSum / Resultado.temperature.onTimeCount;
                Resultado.distanceGroup.onTimeAverage = Grupo_EnTiempo.distanceGroupSum / Resultado.distanceGroup.onTimeCount;
            }

            return Resultado;
        }
    }

}
