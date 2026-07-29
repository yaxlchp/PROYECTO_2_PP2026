#pragma once

#include "FlightDataSet.h"
#include "Models.h"
#include "ParallelAnalyzer.h"
#include "SequentialAnalyzer.h"

namespace airport
{
    class Benchmark
    {
    protected:

        BenchmarkResult run(
            int analysisType,
            const FlightDataSet& dataSet,
            const sequential::SequentialAnalyzer& sequentialAnalyzer,
            const parallel::ParallelAnalyzer& parallelAnalyzer) const;

    protected:
        double executeSequential(
            int analysisType,
            const FlightDataSet& dataSet,
            const sequential::SequentialAnalyzer& analyzer) const;

        double executeParallel(
            int analysisType,
            const FlightDataSet& dataSet,
            const parallel::ParallelAnalyzer& analyzer) const;
    };
}


