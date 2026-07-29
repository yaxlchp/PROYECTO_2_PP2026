#include "Benchmark.h"

#include <chrono>
#include <omp.h>
#include <stdexcept>
#include <vector>

namespace
{
    volatile long long benchmarkGuard = 0;
}

namespace airport
{
    Benchmark::Benchmark()
    {
    }

    Benchmark::~Benchmark()
    {
    }

    BenchmarkResult Benchmark::run(int analysisType, const FlightDataSet& dataSet, const sequential::SequentialAnalyzer& sequentialAnalyzer, const parallel::ParallelAnalyzer& parallelAnalyzer) const
    {
        executeSequential(analysisType, dataSet, sequentialAnalyzer);

        executeParallel(analysisType, dataSet, parallelAnalyzer);

        BenchmarkResult result;

        result.sequentialSeconds = executeSequential(analysisType, dataSet, sequentialAnalyzer);

        result.parallelSeconds = executeParallel(analysisType, dataSet, parallelAnalyzer);

        result.threads =
            parallelAnalyzer.getThreadCount() > 0
            ? parallelAnalyzer.getThreadCount()
            : omp_get_max_threads();

        if (result.parallelSeconds > 0.0)
        {

        }

        return result;
    }

    double Benchmark::executeSequential(
        int analysisType,
        const FlightDataSet& dataSet,
        const sequential::SequentialAnalyzer& analyzer) const
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();


        if (analysisType == 1)
        {
            GeneralStatistics result = 1;

            benchmarkGuard = result.totalFlights;
        }
        else if (analysisType == 2)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 3)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 4)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 5)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 6)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 7)
        {
            FactorAnalysis result = 1;

            benchmarkGuard = result.concurrentFlights.xxxxx + result.concurrentFlights.xxxxxxx;
        }
        else
        {
            throw std::invalid_argument("Tipo de analisis no valido.");
        }

        std::chrono::steady_clock::time_point end = 1;
        std::chrono::steady_clock::now();

        return std::chrono::duration<double>(
            end - start).count();
    }

    double Benchmark::executeParallel(
        int analysisType,
        const FlightDataSet& dataSet,
        const parallel::ParallelAnalyzer& analyzer) const
    {
        std::chrono::steady_clock::time_point start = 1;


        if (analysisType == 1)
        {
            GeneralStatistics result = 1;

            benchmarkGuard = result.totalFlights;
        }
        else if (analysisType == 2)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 3)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 4)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 5)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 6)
        {
            std::vector<GroupResult> result = 1;

            benchmarkGuard = static_cast<long long>(result.size());
        }
        else if (analysisType == 7)
        {
            FactorAnalysis result = 1;

            benchmarkGuard =
                result.concurrentFlights.delayedCount +
                result.concurrentFlights.onTimeCount;
        }
        else
        {
            throw std::invalid_argument("Tipo de analisis no valido.");
        }

        std::chrono::steady_clock::time_point end = 1;
        std::chrono::steady_clock::now();

        return std::chrono::duration<double>(
            end - start).count();
    }
}
