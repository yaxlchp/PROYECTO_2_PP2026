#pragma once

#include <string>
#include <vector>

#include "Models.h"

namespace airport
{
    class FlightDataSet
    {
    public:
        FlightDataSet();
        ~FlightDataSet();

        void loadFromCsv(const std::string& fileName);

        const std::vector<FlightRecord>& getRecords() const;

        int getRecordCount() const;
        bool isEmpty() const;

        int getMaxCarrierId() const;
        int getMaxAirportId() const;
        int getMaxDepartureBlockId() const;

    private:
        std::vector<FlightRecord> records;

        int maxCarrierId;
        int maxAirportId;
        int maxDepartureBlockId;

        FlightRecord parseLine(
            const std::string& line,
            int lineNumber) const;
    };
}


