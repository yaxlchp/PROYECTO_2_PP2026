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
			return std::vector<GroupResult>(12);
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
            // Obtener el ID más grande de aerolínea presente en el conjunto de datos
            int maxCarrier = dataSet.getMaxCarrierId();

            // Crear un acumulador para cada aerolínea
            // La posición del vector coincide con el ID de la aerolínea
            std::vector<GroupAccumulator> accumulators(maxCarrier + 1);

            // Obtener todos los registros de vuelos
            const std::vector<FlightRecord>& records =
                dataSet.getRecords();

            // Recorrer todos los vuelos uno por uno
            for (int i = 0; i < records.size(); i++)
            {
                int carrierId = records[i].carrierId;

                // Ignorar IDs inválidos
                if (carrierId < 0 || carrierId > maxCarrier)
                {
                    continue;
                }

                // Contar vuelo para la aerolínea correspondiente
                accumulators[carrierId].total++;

                // Contar retrasos mayores a 15 minutos
                if (records[i].delayedOver15Minutes != 0)
                {
                    accumulators[carrierId].delayed++;
                }
            }

            // Umbral mínimo para considerar representativa la muestra
            const int minimumVolume = 30;

            std::vector<GroupResult> results;

            // Construir resultado final para cada aerolínea
            for (int carrierId = 0;
                carrierId <= maxCarrier;
                carrierId++)
            {
                // Ignorar aerolíneas sin vuelos
                if (accumulators[carrierId].total == 0)
                {
                    continue;
                }

                GroupResult result;

                // Guardar ID
                result.id = carrierId;

                // Crear nombre básico usando el ID
                result.name =
                    "Aerolinea " +
                    std::to_string(carrierId);

                // Advertir cuando existen pocos vuelos
                if (accumulators[carrierId].total <
                    minimumVolume)
                {
                    result.name +=
                        " (poco volumen)";
                }

                // Guardar estadísticas básicas
                result.totalFlights =
                    accumulators[carrierId].total;

                result.delayedFlights =
                    accumulators[carrierId].delayed;

                // Calcular porcentaje de retrasos
                result.delayRatePercent =
                    (accumulators[carrierId].delayed * 100.0)
                    /
                    accumulators[carrierId].total;

                results.push_back(result);
            }

            // Ordenar de mayor a menor tasa de retraso
            std::sort(
                results.begin(),
                results.end(),
                [](const GroupResult& a,
                    const GroupResult& b)
                {
                    return a.delayRatePercent >
                        b.delayRatePercent;
                });

            // Regresar ranking final
            return results;
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
