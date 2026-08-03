#include "Models.h"

namespace airport
{
/*
      Inicializa todos los atributos del registro de un vuelo con valores
      predeterminados. Los campos enteros se inicializan en 0 y las variables
      climáticas en 0.0f. Esto garantiza que el objeto tenga un estado válido
      antes de cargar información desde un archivo o una base de datos.
     
      Validaciones sugeridas (assert):
      - assert(month >= 1 && month <= 12);
           Verifica que el mes sea válido una vez que se carguen datos reales.
     
      - assert(dayOfWeek >= 1 && dayOfWeek <= 7);
           Comprueba que el día de la semana corresponda a un valor permitido.
     
      - assert(numberOfSeats >= 0);
           Un avión no puede tener una cantidad negativa de asientos.
     
      - assert(planeAge >= 0);
           La antigüedad del avión nunca debe ser negativa.
     */
    FlightRecord::FlightRecord()
        : month(0),
        dayOfWeek(0),
        delayedOver15Minutes(0),
        departureTimeBlockId(0),
        distanceGroup(0),
        concurrentFlights(0),
        numberOfSeats(0),
        carrierId(0),
        airportFlightsMonth(0),
        airlineFlightsMonth(0),
        planeAge(0),
        departingAirportId(0),
        precipitation(0.0f),
        snow(0.0f),
        snowDepth(0.0f),
        maximumTemperature(0.0f),
        averageWindSpeed(0.0f)
    {
    }
/*
     
      Inicializa todas las estadísticas generales en cero.
      Los valores mínimo y máximo de la antigüedad de los aviones se
      inicializan en -1 para indicar que todavía no existen datos
      procesados.
     
      Validaciones sugeridas (assert):
      - assert(totalFlights >= delayedFlights);
           La cantidad de vuelos retrasados nunca puede ser mayor
           que el total de vuelos.
     
      - assert(delayRatePercent >= 0.0 && delayRatePercent <= 100.0);
           El porcentaje de retrasos siempre debe estar entre 0 % y 100 %.
     
      - assert(minimumPlaneAge == -1 || minimumPlaneAge <= maximumPlaneAge);
           Si existen datos, la antigüedad mínima debe ser menor o igual
           que la máxima.
     */
    GeneralStatistics::GeneralStatistics()
        : totalFlights(0),
        delayedFlights(0),
        delayRatePercent(0.0),
        averageConcurrentFlights(0.0),
        averageSeats(0.0),
        averagePlaneAge(0.0),
        averagePrecipitation(0.0),
        averageSnow(0.0),
        averageSnowDepth(0.0),
        averageTemperature(0.0),
        averageWindSpeed(0.0),
        minimumPlaneAge(-1),
        maximumPlaneAge(-1)
    {
    }
/*
      Inicializa la información estadística correspondiente a un grupo
      de vuelos. El nombre se inicializa vacío y todos los indicadores
      numéricos comienzan en cero.
     
      Validaciones sugeridas (assert):
      - assert(totalFlights >= delayedFlights);
           Los vuelos retrasados no pueden superar al total de vuelos.
     
      - assert(delayRatePercent >= 0.0 && delayRatePercent <= 100.0);
           El porcentaje de retrasos debe mantenerse dentro de un rango válido.
     */
    GroupResult:: GroupResult()
        : id(0),
        name(""),
        totalFlights(0),
        delayedFlights(0),
        delayRatePercent(0.0),
        averageConcurrentFlights(0.0),
        averagePlaneAge(0.0),
        averageTemperature(0.0),
        averageWindSpeed(0.0)
    {
    }

    /*
     
      Inicializa los valores utilizados para comparar una característica
      entre vuelos retrasados y vuelos puntuales.
     
      Validaciones sugeridas (assert):
      - assert(delayedCount >= 0);
           La cantidad de vuelos retrasados nunca debe ser negativa.
     
      - assert(onTimeCount >= 0);
           La cantidad de vuelos puntuales tampoco puede ser negativa.
     */
    BinaryComparison::BinaryComparison()
        : factorName(""),
        delayedCount(0),
        onTimeCount(0),
        delayedAverage(0.0),
        onTimeAverage(0.0),
        difference(0.0)
    {
    }
/*
    
      Inicializa las métricas utilizadas para medir el rendimiento del
      algoritmo en ejecución secuencial y paralela.
      El número de hilos comienza en 1, ya que representa la ejecución
      secuencial.
     
      Validaciones sugeridas (assert):
      - assert(sequentialSeconds >= 0.0);
           El tiempo de ejecución nunca puede ser negativo.
     
      - assert(parallelSeconds >= 0.0);
           El tiempo de ejecución paralela tampoco puede ser negativo.
     
      - assert(speedup >= 0.0);
           El speedup representa una razón de rendimiento y no debe ser negativo.
     
      - assert(efficiencyPercent >= 0.0 && efficiencyPercent <= 100.0);
           La eficiencia se expresa como porcentaje y debe estar entre 0 % y 100 %.
     
      - assert(threads >= 1);
           Debe existir al menos un hilo de ejecución.
     */

    BenchmarkResult::BenchmarkResult()
        : sequentialSeconds(0.0),
        parallelSeconds(0.0),
        speedup(0.0),
        efficiencyPercent(0.0),
        threads(1)
    {
    }
}
