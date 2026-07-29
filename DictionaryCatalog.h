#pragma once

#include <string>
#include <vector>

namespace airport
{
    class DictionaryCatalog
    {
    public:
        DictionaryCatalog();
        ~DictionaryCatalog();

        void loadAll(
            const std::string& carriersFile,
            const std::string& airportsFile,
            const std::string& departureBlocksFile);

        std::string getCarrierName(int id) const;
        std::string getAirportName(int id) const;
        std::string getDepartureBlockName(int id) const;

        int getCarrierCount() const;
        int getAirportCount() const;
        int getDepartureBlockCount() const;

    private:
        std::vector<std::string> carrierNames;
        std::vector<std::string> airportNames;
        std::vector<std::string> departureBlockNames;

        void loadDictionary(
            const std::string& fileName,
            std::vector<std::string>& destination);

        std::vector<std::string> splitCsvLine(
            const std::string& line) const;

        std::string findName(
            int id,
            const std::vector<std::string>& values,
            const std::string& fallbackText) const;
    };
}


