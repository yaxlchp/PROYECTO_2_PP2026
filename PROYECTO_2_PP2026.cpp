#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

#include "Menu.h"


int main()
{
    try
    {
        std::string dataFile;
        std::string carriersFile;
        std::string airportsFile;
        std::string departureBlocksFile;

        std::cout
            << "Ruta de flights_small/medium/full.csv: ";
        std::getline(std::cin, dataFile);

        std::cout
            << "Ruta de carriers.csv: ";
        std::getline(std::cin, carriersFile);

        std::cout
            << "Ruta de airports.csv: ";
        std::getline(std::cin, airportsFile);

        std::cout
            << "Ruta de departure_blocks.csv: ";
        std::getline(std::cin, departureBlocksFile);

        airport::DictionaryCatalog catalog;

        catalog.loadAll(
            carriersFile,
            airportsFile,
            departureBlocksFile);

        airport::FlightDataSet dataSet;

        std::chrono::steady_clock::time_point start =
            std::chrono::steady_clock::now();

        dataSet.loadFromCsv(dataFile);

        std::chrono::steady_clock::time_point end =
            std::chrono::steady_clock::now();

        double loadTime =
            std::chrono::duration<double>(
                end - start).count();

        std::cout
            << "\n+------------------------------------------------------------+\n"
            << "| CARGA DE ARCHIVOS                                          |\n"
            << "+-----------------------------+------------------------------+\n"
            << "| Elemento                    | Resultado                    |\n"
            << "+-----------------------------+------------------------------+\n"
            << "| Registros cargados          | "
            << std::setw(28)
            << std::left
            << dataSet.getRecordCount()
            << "|\n"
            << "| Tiempo de carga (s)         | "
            << std::setw(28)
            << std::left
            << std::fixed
            << std::setprecision(4)
            << loadTime
            << "|\n"
            << "| Aerolineas cargadas         | "
            << std::setw(28)
            << std::left
            << catalog.getCarrierCount()
            << "|\n"
            << "| Aeropuertos cargados        | "
            << std::setw(28)
            << std::left
            << catalog.getAirportCount()
            << "|\n"
            << "| Bloques horarios cargados   | "
            << std::setw(28)
            << std::left
            << catalog.getDepartureBlockCount()
            << "|\n"
            << "+-----------------------------+------------------------------+\n";

        airport::UI::Menu menu(
            &dataSet,
            &catalog,
            new airport::sequential::SequentialAnalyzer(),
            new airport::parallel::ParallelAnalyzer(),
            new airport::Benchmark());

        menu.run();

        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "\nERROR: "
            << exception.what()
            << "\n";

        return 1;
    }
}
