#include "DictionaryCatalog.h"
#include <fstream>
#include <sstream>

namespace airport
{
    DictionaryCatalog::DictionaryCatalog() {}
    DictionaryCatalog::~DictionaryCatalog() {}

    void DictionaryCatalog::loadAll(
        const std::string& carriersFile,
        const std::string& airportsFile,
        const std::string& departureBlocksFile)
    {
        loadDictionary(carriersFile, carrierNames);
        loadDictionary(airportsFile, airportNames);
        loadDictionary(departureBlocksFile, departureBlockNames);
    }

    std::string DictionaryCatalog::getCarrierName(int id) const
    {
        return findName(id, carrierNames, "Carrier " + std::to_string(id));
    }

    std::string DictionaryCatalog::getAirportName(int id) const
    {
        return findName(id, airportNames, "Airport " + std::to_string(id));
    }

    std::string DictionaryCatalog::getDepartureBlockName(int id) const
    {
        return findName(id, departureBlockNames, "Block " + std::to_string(id));
    }

    int DictionaryCatalog::getCarrierCount() const
    {
        return carrierNames.size();
    }

    int DictionaryCatalog::getAirportCount() const
    {
        return airportNames.size();
    }

    int DictionaryCatalog::getDepartureBlockCount() const
    {
        return departureBlockNames.size();
    }

    void DictionaryCatalog::loadDictionary(
        const std::string& fileName,
        std::vector<std::string>& destination)
    {
        std::ifstream file(fileName);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line))
        {
            destination.push_back(line);
        }
    }

    std::vector<std::string> DictionaryCatalog::splitCsvLine(
        const std::string& line) const
    {
        std::vector<std::string> result;
        std::stringstream ss(line);
        std::string item;
        while (std::getline(ss, item, ','))
        {
            result.push_back(item);
        }
        return result;
    }

    std::string DictionaryCatalog::findName(
        int id,
        const std::vector<std::string>& values,
        const std::string& fallbackText) const
    {
        if (id >= 0 && id < values.size()) return values[id];
        return fallbackText;
    }
}
