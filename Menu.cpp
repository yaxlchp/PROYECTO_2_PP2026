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
        }

        void Menu::run()
        {
            // run execution
        }
    }
}
