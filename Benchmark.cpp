#include "Benchmark.h"

#include <chrono>
#include <omp.h>
#include <stdexcept>
#include <vector>

namespace
{
    // resultados de los analisis.
    volatile long long benchmarkGuard = 0;
}

namespace airport
{
    BenchmarkResult Benchmark::run(
        int analysisType,
        const FlightDataSet& dataSet,
        const sequential::SequentialAnalyzer& sequentialAnalyzer,
        const parallel::ParallelAnalyzer& parallelAnalyzer) const
    {
        // primera ejecucion
        executeSequential(
            analysisType,
            dataSet,
            sequentialAnalyzer);

        executeParallel(
            analysisType,
            dataSet,
            parallelAnalyzer);

        BenchmarkResult result;

        // medir el tiempo secuencial.
        result.sequentialSeconds =
            executeSequential(
                analysisType,
                dataSet,
                sequentialAnalyzer);

        // medir el tiempo de la paralela.
        result.parallelSeconds =
            executeParallel(
                analysisType,
                dataSet,
                parallelAnalyzer);

        // numero de hilos 
        if (parallelAnalyzer.getThreadCount() > 0)
        {
            result.threads =
                parallelAnalyzer.getThreadCount();
        }
        else
        {
            result.threads =
                omp_get_max_threads();
        }

        // speedup y eficiencia
        if (result.parallelSeconds > 0.0 &&
            result.threads > 0)
        {
            result.speedup =
                result.sequentialSeconds /
                result.parallelSeconds;

            result.efficiencyPercent =
                (result.speedup /
                    static_cast<double>(result.threads))
                * 100.0;
        }
        else
        {
            result.speedup = 0.0;
            result.efficiencyPercent = 0.0;
        }

        return result;
    }

    double Benchmark::executeSequential(
        int analysisType,
        const FlightDataSet& dataSet,
        const sequential::SequentialAnalyzer& analyzer) const
    {
        // tiempo inicial
        std::chrono::steady_clock::time_point start =
            std::chrono::steady_clock::now();

        if (analysisType == 1)
        {
            GeneralStatistics result =
                analyzer.calculateGeneralStatistics(dataSet);

            benchmarkGuard = result.totalFlights;
        }
        else if (analysisType == 2)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByMonth(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 3)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByDayOfWeek(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 4)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByDepartureBlock(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 5)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByCarrier(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 6)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByAirport(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 7)
        {
            FactorAnalysis result =
                analyzer.calculateFactorAnalysis(dataSet);

            benchmarkGuard =
                result.concurrentFlights.delayedCount +
                result.concurrentFlights.onTimeCount;
        }
        else
        {
            throw std::invalid_argument(
                "Tipo de analisis no valido.");
        }

        // tiempo final.
        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();

        // tiempo transcurrido.
        return std::chrono::duration<double>(
            end - start).count();
    }

    double Benchmark::executeParallel(
        int analysisType,
        const FlightDataSet& dataSet,
        const parallel::ParallelAnalyzer& analyzer) const
    {
        //  tiempo inicial.
        std::chrono::steady_clock::time_point start =
            std::chrono::steady_clock::now();

        if (analysisType == 1)
        {
            GeneralStatistics result =
                analyzer.calculateGeneralStatistics(dataSet);

            benchmarkGuard = result.totalFlights;
        }
        else if (analysisType == 2)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByMonth(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 3)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByDayOfWeek(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 4)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByDepartureBlock(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 5)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByCarrier(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 6)
        {
            std::vector<GroupResult> result =
                analyzer.calculateByAirport(dataSet);

            benchmarkGuard =
                static_cast<long long>(result.size());
        }
        else if (analysisType == 7)
        {
            // En el archivo ParallelAnalyzer.h esta funcion
            // no recibe el dataset.
            FactorAnalysis result =
                analyzer.calculateFactorAnalysis();

            benchmarkGuard =
                result.concurrentFlights.delayedCount +
                result.concurrentFlights.onTimeCount;
        }
        else
        {
            throw std::invalid_argument(
                "Tipo de analisis no valido.");
        }

        // tiempo final.
        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();

        // tiempo transcurrido.
        return std::chrono::duration<double>(
            end - start).count();
    }
}