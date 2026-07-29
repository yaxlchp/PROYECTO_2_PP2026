# Asignación del proyecto para 16 alumnos

La lectura de archivos, la carga de diccionarios y la conversión del CSV se entregan terminadas implementadas

## Alumno 1 — Integrador principal - JIMENA CHALÉ PECH

Responsabilidades:

- Crear el repositorio.
- Definir las ramas.
- Integrar cambios en `develop`.
- Resolver conflictos.
- Verificar que el proyecto compile en `Release x64`.
- Revisar que no se hayan agregado plantillas, polimorfismo o sobrecarga de operadores.

Entregable:

- Historial de merges.
- Lista de conflictos resueltos.
- Versión final compilable.

## Alumno 2 — Integrador de validación y métricas - MARIJOSE ROMERO AMADO

Responsabilidades:

- Integrar el menú.
- Integrar el módulo `Benchmark`.
- Verificar coincidencia entre resultados secuenciales y paralelos.
- Ejecutar pruebas con 1, 2, 4, 8 y máximo número de hilos.
- Construir la tabla final de speedup y eficiencia.

Entregable:

- Tabla consolidada de rendimiento.
- Evidencia de igualdad de resultados.

## Alumno 3 — Modelos y estructuras de resultados - JOSÉ EFRAÍN ARTEAFGA MANDUJANO

Responsabilidades:

- Revisar `Models.h` y `Models.cpp`.
- Validar constructores.
- Documentar `FlightRecord`, `GeneralStatistics`, `GroupResult`, `BinaryComparison` y `BenchmarkResult`.
- Proponer validaciones con `assert`.

## Alumno 4 — Excepciones - ÁNGEL EMMANUEL SÁNCHEZ GÓMEZ 

Responsabilidades:

- Revisar las excepciones personalizadas.
- Probar archivo inexistente.
- Probar dataset vacío.
- Probar línea con columnas incompletas.
- Documentar cuándo se usa cada excepción.

## Alumno 5 — Estadísticas generales secuenciales - ARI MARTÍNEZ CARRIZALES

Responsabilidades:

- Implementar y probar `SequentialAnalyzer::calculateGeneralStatistics`.
- Validar conteos, promedios, mínimo y máximo.
- Documentar complejidad temporal.

Pregunta de análisis:

> ¿Cuál es la magnitud global de los retrasos y cuáles son las condiciones promedio de operación?

## Alumno 6 — Estadísticas generales paralelas - PEDRO MEZA FLORES

Responsabilidades:

- Implementar y probar `ParallelAnalyzer::calculateGeneralStatistics`.
- Usar `reduction`.
- Explicar el manejo de mínimo y máximo por hilo.
- Comparar contra la versión secuencial.

## Alumno 7 — Análisis por mes secuencial y paralelo - JHONATHAN RIVAS GUERRERO

Responsabilidades:

- Implementar ambas versiones de `calculateByMonth`.
- Verificar los 12 grupos.
- Identificar mes con más vuelos, más retrasos y mayor tasa.

Pregunta:

> ¿Existe un patrón estacional en los retrasos?

## Alumno 8 — Análisis por día de la semana - ANDRÉS IBARRA PAZ

Responsabilidades:

- Implementar ambas versiones de `calculateByDayOfWeek`.
- Validar los siete días.
- Comparar volumen, concurrencia y tasa de retraso.

Pregunta:

> ¿El día con mayor volumen también es el día con mayor tasa de retraso?

## Alumno 9 — Análisis por bloque horario - VICTOR ZAVALA MENDOZA

Responsabilidades:

- Implementar ambas versiones de `calculateByDepartureBlock`.
- Validar la relación entre ID y nombre del bloque.
- Identificar acumulación de retrasos durante el día.

Pregunta:

> ¿Los retrasos aumentan conforme avanza el día?

## Alumno 10 — Análisis por aerolínea - VIVIANA MARTÍNEZ LÓPEZ

Responsabilidades:

- Implementar ambas versiones de `calculateByCarrier`.
- Mostrar ID y nombre de aerolínea.
- Construir ranking por tasa de retraso.
- Advertir sobre aerolíneas con poco volumen.

Pregunta:

> ¿Qué aerolíneas presentan mayor tasa de retraso y es justo compararlas sin considerar el volumen?

## Alumno 11 — Análisis por aeropuerto - CARLOS MONTENEGRO TAPIA

Responsabilidades:

- Implementar ambas versiones de `calculateByAirport`.
- Mostrar ID y nombre de aeropuerto.
- Construir ranking por tasa.
- Proponer un mínimo de vuelos para considerar confiable un aeropuerto.

Pregunta:

> ¿Qué aeropuertos concentran los mayores problemas de puntualidad?

## Alumno 12 — Factores asociados secuenciales - LUIGI VELÁQUEZ VEGA

Responsabilidades:

- Implementar `SequentialAnalyzer::calculateFactorAnalysis`.
- Comparar vuelos retrasados y puntuales.
- Validar los seis factores.

Pregunta:

> ¿Qué variables muestran las mayores diferencias promedio entre vuelos retrasados y puntuales?

## Alumno 13 — Factores asociados paralelos - JOAQUÍN CASARÍN GONZÁLEZ

Responsabilidades:

- Implementar `ParallelAnalyzer::calculateFactorAnalysis`.
- Usar reducciones de OpenMP.
- Comparar resultados con la versión secuencial.
- Explicar por qué diferencia de promedios no significa causalidad.

## Alumno 14 — UI de tablas - GERARDO MARTÍNEZ MORENO

Responsabilidades:

- Mantener el menú principal.
- Implementar tablas claras.
- Revisar alineación de columnas.
- Mostrar títulos de versión secuencial/paralela.
- Mostrar nombres de aeropuertos, aerolíneas y bloques horarios.

## Alumno 15 — Benchmark y presentación de resultados - NAHOMY LÓPEZ RIVADENEYRA

Responsabilidades:

- Revisar la clase `Benchmark`.
- Mostrar la tabla del resultado de la operación evaluada.
- Mostrar la tabla resumen con tiempo, hilos, speedup y eficiencia.
- Probar distintos tamaños de dataset.

## Alumno 16 — Pruebas funcionales y documentación - ARTURO PELCASTRE HERNÁNDEZ

Responsabilidades:

- Crear casos de prueba.
- Ejecutar `small`, `medium` y `full`.
- Documentar errores encontrados.
- Preparar README final.
- Coordinar capturas y evidencias para la exposición.

# Reglas comunes

Cada alumno debe:

1. Trabajar en una rama propia.
2. Implementar versión secuencial y/o paralela según la asignación.
3. Agregar al menos una prueba.
4. Documentar el pragma OpenMP utilizado.
5. Explicar riesgos de condición de carrera.
6. Medir en `Release x64`.
7. No modificar la lectura de archivos.
8. No utilizar plantillas propias.
9. No utilizar polimorfismo.
10. No utilizar sobrecarga de operadores.
