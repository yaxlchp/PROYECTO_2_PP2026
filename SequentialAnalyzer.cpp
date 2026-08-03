#include "SequentialAnalyzer.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

#include "Exceptions.h"

namespace
{
    struct GroupAccumulator
    {
        long long total;
        long long delayed;
        double concurrentSum;
        double planeAgeSum;
        long long validPlaneAgeCount;
        double temperatureSum;
        double windSum;

        GroupAccumulator()
            : total(0),
            delayed(0),
            concurrentSum(0.0),
            planeAgeSum(0.0),
            validPlaneAgeCount(0),
            temperatureSum(0.0),
            windSum(0.0)
        {
        }
    };

}


namespace airport
{
    namespace sequential
    {
        SequentialAnalyzer::SequentialAnalyzer()
        {
        }

        SequentialAnalyzer::~SequentialAnalyzer()
        {
        }

        GeneralStatistics SequentialAnalyzer::calculateGeneralStatistics(
            const FlightDataSet& dataSet) const
        {
			return GeneralStatistics();
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByMonth(
            const FlightDataSet& dataSet) const
        {
            std::vector<GroupResult> result(12);
            std::vector<GroupAccumulator> accum(12);

            std::string monthName[12] =
            {
                "January", "February", "March", "April",
                "May", "June", "July", "August",
                "September", "October", "November", "December"
            };

            for (int i = 0; i < 12; i++)
            {
                result[i].name = monthName[i];
            }

            int n = dataSet.getRecordCount();
            const std::vector<FlightRecord>& records = dataSet.getRecords();
            
            for (int i = 0; i < n; i++)
            {
                const FlightRecord& record = records[i];
                GroupAccumulator& month = accum[record.month - 1];

                month.total++;
                if (record.delayedOver15Minutes) month.delayed++;
                month.concurrentSum += record.concurrentFlights;
                if (record.planeAge>=0 && record.planeAge <= 100)
                {
                    month.planeAgeSum += record.planeAge;
                    month.validPlaneAgeCount++;
                }
                month.temperatureSum += record.maximumTemperature;
                month.windSum += record.averageWindSpeed;
            }

            for (int i = 0; i < 12; i++)
            {
                result[i].id = i + 1;
                result[i].totalFlights = accum[i].total;
                result[i].delayedFlights = accum[i].delayed;
                if (accum[i].total > 0)
                {
                    result[i].delayRatePercent = 100.0 * accum[i].delayed / accum[i].total ;
                    result[i].averageConcurrentFlights = accum[i].concurrentSum / accum[i].total;
                    if (accum[i].validPlaneAgeCount>0) result[i].averagePlaneAge = accum[i].planeAgeSum / accum[i].validPlaneAgeCount;
                    result[i].averageTemperature = accum[i].temperatureSum / accum[i].total;
                    result[i].averageWindSpeed = accum[i].windSum / accum[i].total;
                }
            }
			return result;
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByDayOfWeek(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(7);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByDepartureBlock(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxDepartureBlockId() + 1);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByCarrier(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxCarrierId() + 1);
        }

        std::vector<GroupResult> SequentialAnalyzer::calculateByAirport(
            const FlightDataSet& dataSet) const
        {
			return std::vector<GroupResult>(dataSet.getMaxAirportId() + 1);
        }


        FactorAnalysis SequentialAnalyzer::calculateFactorAnalysis(
            const FlightDataSet& dataSet) const
        {
			return FactorAnalysis();
        }

    }
}
