#pragma once

#include <stdexcept>
#include <string>

namespace airport
{
    namespace exceptions
    {
        class AirportException : public std::runtime_error
        {
        public:
            explicit AirportException(const std::string& message);
            virtual ~AirportException();
        };

        class FileOpenException : public AirportException
        {
        public:
            explicit FileOpenException(const std::string& fileName);
            ~FileOpenException();
        };

        class CsvFormatException : public AirportException
        {
        public:
            CsvFormatException(int lineNumber, const std::string& detail);
            ~CsvFormatException();
        };

        class EmptyDataSetException : public AirportException
        {
        public:
            EmptyDataSetException();
            ~EmptyDataSetException();
        };
    }
}


