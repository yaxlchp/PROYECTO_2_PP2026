#include "FlightDataSet.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Exceptions.h"

namespace airport
{
    FlightDataSet::FlightDataSet()
        : maxCarrierId(0),
        maxAirportId(0),
        maxDepartureBlockId(0)
    {
    }

    FlightDataSet::~FlightDataSet()
    {
    }

    void FlightDataSet::loadFromCsv(const std::string& fileName)
    {
        std::ifstream file(fileName);

        if (!file.is_open())
        {
            throw exceptions::FileOpenException(fileName);
        }

        records.clear();
        maxCarrierId = 0;
        maxAirportId = 0;
        maxDepartureBlockId = 0;

        std::string line;
        int lineNumber = 0;

        if (!std::getline(file, line))
        {
            throw exceptions::EmptyDataSetException();
        }

        lineNumber++;
        records.reserve(1000000);

        while (std::getline(file, line))
        {
            lineNumber++;

            if (line.empty())
            {
                continue;
            }

            FlightRecord record = parseLine(line, lineNumber);

            records.push_back(record);

            maxCarrierId =
                std::max(maxCarrierId, record.carrierId);

            maxAirportId =
                std::max(maxAirportId, record.departingAirportId);

            maxDepartureBlockId =
                std::max(
                    maxDepartureBlockId,
                    record.departureTimeBlockId);
        }

        if (records.empty())
        {
            throw exceptions::EmptyDataSetException();
        }

        assert(maxCarrierId >= 0);
        assert(maxAirportId >= 0);
        assert(maxDepartureBlockId >= 0);
    }

    const std::vector<FlightRecord>&
        FlightDataSet::getRecords() const
    {
        return records;
    }

    int FlightDataSet::getRecordCount() const
    {
        return static_cast<int>(records.size());
    }

    bool FlightDataSet::isEmpty() const
    {
        return records.empty();
    }

    int FlightDataSet::getMaxCarrierId() const
    {
        return maxCarrierId;
    }

    int FlightDataSet::getMaxAirportId() const
    {
        return maxAirportId;
    }

    int FlightDataSet::getMaxDepartureBlockId() const
    {
        return maxDepartureBlockId;
    }

    FlightRecord FlightDataSet::parseLine(
        const std::string& line,
        int lineNumber) const
    {
        std::stringstream stream(line);
        std::string field;
        std::vector<std::string> fields;

        while (std::getline(stream, field, ','))
        {
            fields.push_back(field);
        }

        if (fields.size() != 17)
        {
            throw exceptions::CsvFormatException(
                lineNumber,
                "Se esperaban 17 columnas y se encontraron " +
                std::to_string(fields.size()) + ".");
        }

        FlightRecord record;

        try
        {
            // Esta variable contiene el mes del vuelo.
            record.month = std::stoi(fields[0]);

            // Esta variable contiene el dia de la semana.
            record.dayOfWeek = std::stoi(fields[1]);

            // Esta variable indica si el vuelo tuvo retraso mayor a 15 minutos.
            record.delayedOver15Minutes = std::stoi(fields[2]);

            // Esta variable contiene el ID del bloque horario de salida.
            record.departureTimeBlockId = std::stoi(fields[3]);

            // Esta variable contiene el grupo de distancia del vuelo.
            record.distanceGroup = std::stoi(fields[4]);

            // Esta variable contiene la cantidad de vuelos concurrentes.
            record.concurrentFlights = std::stoi(fields[5]);

            // Esta variable contiene la cantidad de asientos del avion.
            record.numberOfSeats = std::stoi(fields[6]);

            // Esta variable contiene el ID de aerolinea.
            record.carrierId = std::stoi(fields[7]);

            // Esta variable contiene los vuelos mensuales del aeropuerto.
            record.airportFlightsMonth = std::stoi(fields[8]);

            // Esta variable contiene los vuelos mensuales de la aerolinea.
            record.airlineFlightsMonth = std::stoi(fields[9]);

            // Esta variable contiene la edad del avion.
            record.planeAge = std::stoi(fields[10]);

            // Esta variable contiene el ID de aeropuerto.
            record.departingAirportId = std::stoi(fields[11]);

            // Esta variable contiene la precipitacion.
            record.precipitation = std::stof(fields[12]);

            // Esta variable contiene la cantidad de nieve.
            record.snow = std::stof(fields[13]);

            // Esta variable contiene la nieve acumulada.
            record.snowDepth = std::stof(fields[14]);

            // Esta variable contiene temperatura.
            record.maximumTemperature = std::stof(fields[15]);

            // Esta variable contiene la velocidad promedio del viento.
            record.averageWindSpeed = std::stof(fields[16]);
        }
        catch (const std::exception&)
        {
            throw exceptions::CsvFormatException(
                lineNumber,
                "Uno o mas valores no son numericos.");
        }

        assert(record.month >= 1 && record.month <= 12);
        assert(record.dayOfWeek >= 1 && record.dayOfWeek <= 7);
        assert(
            record.delayedOver15Minutes == 0 ||
            record.delayedOver15Minutes == 1);

        return record;
    }
}
