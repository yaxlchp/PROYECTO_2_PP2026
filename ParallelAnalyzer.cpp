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
        double distanceGrupoSum; //Varibale para tomar la distancia de vuelo promedio de lo vuelos.

        GroupAccumulator()
            : total(0),
            delayed(0),
            concurrentSum(0.0),
            planeAgeSum(0.0),
            validPlaneAgeCount(0),
            temperatureSum(0.0),
            windSum(0.0),
            precipitationSum(0.0), //Promedio de precipitaciones
            distanceGrupoSum(0.0) //Promedio de distancia
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
