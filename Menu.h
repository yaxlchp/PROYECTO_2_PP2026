#pragma once

#include <string>
#include <vector>

#include "Benchmark.h"
#include "DictionaryCatalog.h"
#include "FlightDataSet.h"
#include "ParallelAnalyzer.h"
#include "SequentialAnalyzer.h"

namespace airport
{
    namespace UI
    {
        class Menu
        {
        public:
            Menu(
                airport::FlightDataSet* dataSet,
                airport::DictionaryCatalog* catalog,
                airport::sequential::SequentialAnalyzer* sequentialAnalyzer,
                airport::parallel::ParallelAnalyzer* parallelAnalyzer,
                airport::Benchmark* benchmark);

            ~Menu();

            void run();

        protected:
            airport::FlightDataSet* dataSet;
            airport::DictionaryCatalog* catalog;
            airport::sequential::SequentialAnalyzer* sequentialAnalyzer;
            airport::parallel::ParallelAnalyzer* parallelAnalyzer;
            airport::Benchmark* benchmark;

            void showMainMenu() const;
            void executeAnalysis(int option);
            void executeBenchmark();
            void configureThreads();

            void assignNames(
                std::vector<airport::GroupResult>& results,
                int analysisType) const;

            void sortByDelayRate(
                std::vector<airport::GroupResult>& results) const;

            void printGeneralStatisticsTable(
                const airport::GeneralStatistics& result,
                const std::string& title) const;

            void printGroupedResultsTable(
                const std::vector<airport::GroupResult>& results,
                const std::string& title,
                int maximumRows) const;

            void printFactorAnalysisTable(
                const airport::FactorAnalysis& result,
                const std::string& title) const;

            void printBenchmarkSummaryTable(
                const airport::BenchmarkResult& result) const;

            void printBenchmarkOperationResult() const;
        };
    }
}



