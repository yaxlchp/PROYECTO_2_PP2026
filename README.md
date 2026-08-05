# PROYECTO 2 PP2026

Aplicacion de consola para analizar un dataset de vuelos en formato CSV. El programa carga el archivo principal de vuelos, lee los catalogos de aerolineas, aeropuertos y bloques horarios, y despues permite ejecutar analisis secuenciales, paralelos y benchmark desde un menu interactivo.

## Requisitos

- Visual Studio con C++ para escritorio.
- Configuracion `Release x64`.
- OpenMP habilitado por el proyecto.

## Ejecucion

1. Abrir la solucion `PROYECTO_2_PP2026.slnx`.
2. Seleccionar la configuracion `Release` y la plataforma `x64`.
3. Compilar y ejecutar el proyecto.
4. Cuando el programa lo solicite, capturar las rutas de:
   - `flights_small.csv`, `flights_medium.csv` o el dataset completo.
   - `carriers.csv`.
   - `airports.csv`.
   - `departure_blocks.csv`.
5. Elegir una opcion del menu principal.

## Menu principal

- 1. Estadisticas generales
- 2. Analisis por mes
- 3. Analisis por dia de la semana
- 4. Analisis por bloque de salida
- 5. Analisis por aerolinea
- 6. Analisis por aeropuerto
- 7. Analisis de factores
- 8. Ejecutar benchmark
- 9. Configurar numero de hilos
- 0. Salir

## Criterios de validacion para el alumno 16

La evidencia funcional debe cubrir al menos estos escenarios:

- Carga correcta con `small` y `medium`.
- Archivo de vuelos inexistente.
- Dataset vacio.
- Linea CSV con columnas incompletas o valores no numericos.
- Opcion invalida en el menu.
- Configuracion invalida de hilos.
- Ejecucion de benchmark con un analisis representativo.

La lista completa de casos esta en [PRUEBAS_FUNCIONALES.md](PRUEBAS_FUNCIONALES.md).

## Observaciones tecnicas

- El programa valida el archivo principal de vuelos con excepciones personalizadas.
- Los catalogos se cargan por separado y, si un archivo auxiliar no existe, el sistema usa nombres de respaldo.
- El benchmark compara tiempos secuenciales y paralelos y reporta speedup y eficiencia.
