#include "Menu.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
    int readInteger(const std::string& message)
    {
        while (true)
        {
            std::cout << message;

            int value;

            if (std::cin >> value)
            {
                return value;
            }

            std::cin.clear();

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n');

            std::cout
                << "Entrada invalida. Intente nuevamente.\n";
        }
    }

    bool chooseParallel()
    {
        int option = readInteger(
            "\n+-----------------------------+\n"
            "| VERSION A EJECUTAR          |\n"
            "+-----------------------------+\n"
            "| 1. Secuencial               |\n"
            "| 2. Paralela                 |\n"
            "+-----------------------------+\n"
            "Seleccione: ");

        return option == 2;
    }

    /*
        Benchmark::run es protected en Benchmark.h.

        Esta clase pequeña permite utilizarlo sin modificar
        Benchmark.h ni main.cpp.
    */
    class BenchmarkAccess : public airport::Benchmark
    {
    public:
        airport::BenchmarkResult execute(
            int analysisType,
            const airport::FlightDataSet& dataSet,
            const airport::sequential::SequentialAnalyzer& sequentialAnalyzer,
            const airport::parallel::ParallelAnalyzer& parallelAnalyzer) const
        {
            return run(
                analysisType,
                dataSet,
                sequentialAnalyzer,
                parallelAnalyzer);
        }
    };

    // Imprime una fila de la tabla de factores.
    void printFactorRow(
        const airport::BinaryComparison& factor)
    {
        std::cout
            << "| "
            << std::setw(20)
            << std::left
            << factor.factorName
            << "| "
            << std::setw(12)
            << factor.delayedAverage
            << "| "
            << std::setw(12)
            << factor.onTimeAverage
            << "| "
            << std::setw(12)
            << factor.difference
            << "|\n";
    }
}

namespace airport
{
    namespace UI
    {
        Menu::Menu(
            airport::FlightDataSet* dataSet,
            airport::DictionaryCatalog* catalog,
            airport::sequential::SequentialAnalyzer* sequentialAnalyzer,
            airport::parallel::ParallelAnalyzer* parallelAnalyzer,
            airport::Benchmark* benchmark)
            : dataSet(dataSet),
            catalog(catalog),
            sequentialAnalyzer(sequentialAnalyzer),
            parallelAnalyzer(parallelAnalyzer),
            benchmark(benchmark)
        {
        }

        Menu::~Menu()
        {
            // Estos objetos fueron creados con new en main.
            delete sequentialAnalyzer;
            delete parallelAnalyzer;
            delete benchmark;
        }

        void Menu::run()
        {
            int option = -1;

            while (option != 0)
            {
                showMainMenu();

                option = readInteger("Seleccione una opcion: ");

                try
                {
                    if (option >= 1 && option <= 7)
                    {
                        executeAnalysis(option);
                    }
                    else if (option == 8)
                    {
                        executeBenchmark();
                    }
                    else if (option == 9)
                    {
                        configureThreads();
                    }
                    else if (option == 0)
                    {
                        std::cout
                            << "\nPrograma terminado.\n";
                    }
                    else
                    {
                        std::cout
                            << "\nOpcion no valida.\n";
                    }
                }
                catch (const std::exception& exception)
                {
                    std::cout
                        << "\nERROR: "
                        << exception.what()
                        << "\n";
                }
            }
        }

        void Menu::showMainMenu() const
        {
            std::cout
                << "\n+--------------------------------------+\n"
                << "|            MENU PRINCIPAL            |\n"
                << "+--------------------------------------+\n"
                << "| 1. Estadisticas generales            |\n"
                << "| 2. Analisis por mes                  |\n"
                << "| 3. Analisis por dia de la semana     |\n"
                << "| 4. Analisis por bloque de salida     |\n"
                << "| 5. Analisis por aerolinea            |\n"
                << "| 6. Analisis por aeropuerto           |\n"
                << "| 7. Analisis de factores              |\n"
                << "| 8. Ejecutar benchmark                |\n"
                << "| 9. Configurar numero de hilos        |\n"
                << "| 0. Salir                             |\n"
                << "+--------------------------------------+\n";
        }

        void Menu::executeAnalysis(int option)
        {
            bool useParallel = chooseParallel();

            if (option == 1)
            {
                GeneralStatistics result;

                if (useParallel)
                {
                    result =
                        parallelAnalyzer->
                        calculateGeneralStatistics(*dataSet);
                }
                else
                {
                    result =
                        sequentialAnalyzer->
                        calculateGeneralStatistics(*dataSet);
                }

                printGeneralStatisticsTable(
                    result,
                    "ESTADISTICAS GENERALES");
            }
            else if (option >= 2 && option <= 6)
            {
                std::vector<GroupResult> results;

                if (useParallel)
                {
                    if (option == 2)
                    {
                        results =
                            parallelAnalyzer->
                            calculateByMonth(*dataSet);
                    }
                    else if (option == 3)
                    {
                        results =
                            parallelAnalyzer->
                            calculateByDayOfWeek(*dataSet);
                    }
                    else if (option == 4)
                    {
                        results =
                            parallelAnalyzer->
                            calculateByDepartureBlock(*dataSet);
                    }
                    else if (option == 5)
                    {
                        results =
                            parallelAnalyzer->
                            calculateByCarrier(*dataSet);
                    }
                    else if (option == 6)
                    {
                        results =
                            parallelAnalyzer->
                            calculateByAirport(*dataSet);
                    }
                }
                else
                {
                    if (option == 2)
                    {
                        results =
                            sequentialAnalyzer->
                            calculateByMonth(*dataSet);
                    }
                    else if (option == 3)
                    {
                        results =
                            sequentialAnalyzer->
                            calculateByDayOfWeek(*dataSet);
                    }
                    else if (option == 4)
                    {
                        results =
                            sequentialAnalyzer->
                            calculateByDepartureBlock(*dataSet);
                    }
                    else if (option == 5)
                    {
                        results =
                            sequentialAnalyzer->
                            calculateByCarrier(*dataSet);
                    }
                    else if (option == 6)
                    {
                        results =
                            sequentialAnalyzer->
                            calculateByAirport(*dataSet);
                    }
                }

                assignNames(results, option);
                sortByDelayRate(results);

                std::string title;

                if (option == 2)
                {
                    title = "ANALISIS POR MES";
                }
                else if (option == 3)
                {
                    title = "ANALISIS POR DIA DE LA SEMANA";
                }
                else if (option == 4)
                {
                    title = "ANALISIS POR BLOQUE DE SALIDA";
                }
                else if (option == 5)
                {
                    title = "ANALISIS POR AEROLINEA";
                }
                else
                {
                    title = "ANALISIS POR AEROPUERTO";
                }

                printGroupedResultsTable(
                    results,
                    title,
                    20);
            }
            else if (option == 7)
            {
                FactorAnalysis result;

                if (useParallel)
                {
                    result =
                        parallelAnalyzer->
                        calculateFactorAnalysis();
                }
                else
                {
                    result =
                        sequentialAnalyzer->
                        calculateFactorAnalysis(*dataSet);
                }

                printFactorAnalysisTable(
                    result,
                    "ANALISIS DE FACTORES");
            }
        }

        void Menu::executeBenchmark()
        {
            int analysisType = readInteger(
                "\n+--------------------------------------+\n"
                "| ANALISIS PARA EL BENCHMARK           |\n"
                "+--------------------------------------+\n"
                "| 1. Estadisticas generales            |\n"
                "| 2. Por mes                           |\n"
                "| 3. Por dia de la semana              |\n"
                "| 4. Por bloque de salida              |\n"
                "| 5. Por aerolinea                     |\n"
                "| 6. Por aeropuerto                    |\n"
                "| 7. Analisis de factores              |\n"
                "+--------------------------------------+\n"
                "Seleccione: ");

            if (analysisType < 1 || analysisType > 7)
            {
                std::cout
                    << "\nTipo de analisis no valido.\n";

                return;
            }

            /*
                Primero se muestra la tabla de la operacion.
                Esto cumple con la presentacion del resultado.
            */
            std::cout
                << "\nRESULTADO DE LA OPERACION EVALUADA\n";

            executeAnalysis(analysisType);

            // Ejecutar las mediciones.
            BenchmarkAccess benchmarkAccess;

            BenchmarkResult result =
                benchmarkAccess.execute(
                    analysisType,
                    *dataSet,
                    *sequentialAnalyzer,
                    *parallelAnalyzer);

            // Mostrar la tabla de tiempos.
            printBenchmarkSummaryTable(result);
        }

        void Menu::configureThreads()
        {
            int threads = readInteger(
                "\nNumero de hilos: ");

            if (threads <= 0)
            {
                std::cout
                    << "El numero de hilos debe ser mayor que cero.\n";

                return;
            }

            parallelAnalyzer->setThreadCount(threads);

            std::cout
                << "Numero de hilos configurado: "
                << threads
                << "\n";
        }

        void Menu::assignNames(
            std::vector<GroupResult>& results,
            int analysisType) const
        {
            const char* monthNames[12] =
            {
                "Enero",
                "Febrero",
                "Marzo",
                "Abril",
                "Mayo",
                "Junio",
                "Julio",
                "Agosto",
                "Septiembre",
                "Octubre",
                "Noviembre",
                "Diciembre"
            };

            for (int i = 0;
                i < static_cast<int>(results.size());
                ++i)
            {
                if (!results[i].name.empty())
                {
                    continue;
                }

                if (analysisType == 2 &&
                    results[i].id >= 1 &&
                    results[i].id <= 12)
                {
                    results[i].name =
                        monthNames[results[i].id - 1];
                }
                else if (analysisType == 3)
                {
                    results[i].name =
                        "Dia " +
                        std::to_string(results[i].id);
                }
                else if (analysisType == 4)
                {
                    results[i].name =
                        "Bloque " +
                        std::to_string(results[i].id);
                }
                else if (analysisType == 5)
                {
                    results[i].name =
                        "Aerolinea " +
                        std::to_string(results[i].id);
                }
                else if (analysisType == 6)
                {
                    results[i].name =
                        "Aeropuerto " +
                        std::to_string(results[i].id);
                }
            }
        }

        void Menu::sortByDelayRate(
            std::vector<GroupResult>& results) const
        {
            // Ordenamiento sencillo de mayor a menor.
            for (int i = 0;
                i < static_cast<int>(results.size());
                ++i)
            {
                for (int j = 0;
                    j < static_cast<int>(results.size()) - 1;
                    ++j)
                {
                    if (results[j].delayRatePercent <
                        results[j + 1].delayRatePercent)
                    {
                        GroupResult temporary =
                            results[j];

                        results[j] =
                            results[j + 1];

                        results[j + 1] =
                            temporary;
                    }
                }
            }
        }

        void Menu::printGeneralStatisticsTable(
            const GeneralStatistics& result,
            const std::string& title) const
        {
            std::cout
                << "\n"
                << title
                << "\n";

            std::cout
                << "+-------------------------------+----------------+\n"
                << "| Dato                          | Resultado      |\n"
                << "+-------------------------------+----------------+\n";

            std::cout
                << "| Total de vuelos               | "
                << std::setw(14)
                << result.totalFlights
                << " |\n";

            std::cout
                << "| Vuelos retrasados             | "
                << std::setw(14)
                << result.delayedFlights
                << " |\n";

            std::cout
                << "| Tasa de retraso (%)           | "
                << std::setw(14)
                << std::fixed
                << std::setprecision(2)
                << result.delayRatePercent
                << " |\n";

            std::cout
                << "| Promedio vuelos concurrentes  | "
                << std::setw(14)
                << result.averageConcurrentFlights
                << " |\n";

            std::cout
                << "| Promedio de asientos          | "
                << std::setw(14)
                << result.averageSeats
                << " |\n";

            std::cout
                << "| Promedio edad del avion       | "
                << std::setw(14)
                << result.averagePlaneAge
                << " |\n";

            std::cout
                << "| Temperatura promedio          | "
                << std::setw(14)
                << result.averageTemperature
                << " |\n";

            std::cout
                << "| Velocidad promedio del viento | "
                << std::setw(14)
                << result.averageWindSpeed
                << " |\n";

            std::cout
                << "+-------------------------------+----------------+\n";
        }

        void Menu::printGroupedResultsTable(
            const std::vector<GroupResult>& results,
            const std::string& title,
            int maximumRows) const
        {
            std::cout
                << "\n"
                << title
                << "\n";

            std::cout
                << "+------+--------------------------+------------+------------+------------+\n"
                << "| ID   | Nombre                   | Vuelos     | Retrasos   | Tasa (%)   |\n"
                << "+------+--------------------------+------------+------------+------------+\n";

            int rowsToShow = maximumRows;

            if (rowsToShow <= 0 ||
                rowsToShow >
                static_cast<int>(results.size()))
            {
                rowsToShow =
                    static_cast<int>(results.size());
            }

            for (int i = 0; i < rowsToShow; ++i)
            {
                std::cout
                    << "| "
                    << std::setw(4)
                    << std::left
                    << results[i].id
                    << " | "
                    << std::setw(24)
                    << results[i].name
                    << " | "
                    << std::setw(10)
                    << results[i].totalFlights
                    << " | "
                    << std::setw(10)
                    << results[i].delayedFlights
                    << " | "
                    << std::setw(10)
                    << std::fixed
                    << std::setprecision(2)
                    << results[i].delayRatePercent
                    << " |\n";
            }

            std::cout
                << "+------+--------------------------+------------+------------+------------+\n";

            if (rowsToShow <
                static_cast<int>(results.size()))
            {
                std::cout
                    << "Se mostraron "
                    << rowsToShow
                    << " de "
                    << results.size()
                    << " resultados.\n";
            }
        }

        void Menu::printFactorAnalysisTable(
            const FactorAnalysis& result,
            const std::string& title) const
        {
            std::cout
                << "\n"
                << title
                << "\n";

            std::cout
                << "+---------------------+-------------+-------------+-------------+\n"
                << "| Factor              | Retrasados  | A tiempo    | Diferencia  |\n"
                << "+---------------------+-------------+-------------+-------------+\n";

            printFactorRow(result.concurrentFlights);
            printFactorRow(result.planeAge);
            printFactorRow(result.precipitation);
            printFactorRow(result.windSpeed);
            printFactorRow(result.temperature);
            printFactorRow(result.distanceGroup);

            std::cout
                << "+---------------------+-------------+-------------+-------------+\n";
        }

        void Menu::printBenchmarkSummaryTable(
            const BenchmarkResult& result) const
        {
            std::cout
                << "\n+------------------------------------------------------+\n"
                << "|              RESUMEN DEL BENCHMARK                  |\n"
                << "+----------------------------+-------------------------+\n";

            std::cout
                << "| Registros del dataset      | "
                << std::setw(23)
                << dataSet->getRecordCount()
                << " |\n";

            std::cout
                << "| Tiempo secuencial (s)      | "
                << std::setw(23)
                << std::fixed
                << std::setprecision(6)
                << result.sequentialSeconds
                << " |\n";

            std::cout
                << "| Tiempo paralelo (s)        | "
                << std::setw(23)
                << result.parallelSeconds
                << " |\n";

            std::cout
                << "| Numero de hilos            | "
                << std::setw(23)
                << result.threads
                << " |\n";

            std::cout
                << "| Speedup                    | "
                << std::setw(23)
                << std::setprecision(3)
                << result.speedup
                << " |\n";

            std::cout
                << "| Eficiencia (%)             | "
                << std::setw(23)
                << result.efficiencyPercent
                << " |\n";

            std::cout
                << "+----------------------------+-------------------------+\n";
        }

        void Menu::printBenchmarkOperationResult() const
        {
            std::cout
                << "\nLa tabla de la operacion se muestra "
                << "antes del resumen del benchmark.\n";
        }
    }
}
