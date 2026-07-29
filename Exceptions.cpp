#include "Exceptions.h"

namespace airport
{
    namespace exceptions
    {
        AirportException::AirportException(const std::string& message) : std::runtime_error(message) {}
        AirportException::~AirportException() = default;

        FileOpenException::FileOpenException(const std::string& fileName) : AirportException("No se pudo abrir el archivo: " + fileName) {}
        FileOpenException::~FileOpenException() = default;

        CsvFormatException::CsvFormatException(int lineNumber, const std::string& detail) : AirportException("Error de formato CSV en linea " + std::to_string(lineNumber) + ": " + detail) {}
        CsvFormatException::~CsvFormatException() = default;

        EmptyDataSetException::EmptyDataSetException() : AirportException("El dataset esta vacio.") {}
        EmptyDataSetException::~EmptyDataSetException() = default;
    }
}
