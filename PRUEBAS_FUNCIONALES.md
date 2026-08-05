# Pruebas funcionales

Este documento resume los casos que corresponden al alumno 16: ejecucion con datasets distintos, documentacion de errores y evidencia para el README final.

## Matriz de pruebas

| ID | Escenario | Entrada | Resultado esperado |
| --- | --- | --- | --- |
| PF-01 | Carga valida con dataset pequeno | `flights_small.csv` + catalogos correctos | El programa muestra la tabla de carga y permite usar el menu principal. |
| PF-02 | Carga valida con dataset mediano | `flights_medium.csv` + catalogos correctos | La carga termina sin excepciones y el menu responde normalmente. |
| PF-03 | Archivo de vuelos inexistente | Ruta invalida para `flights_*.csv` | Se muestra `No se pudo abrir el archivo: ...`. |
| PF-04 | Dataset vacio | CSV sin registros de datos | Se lanza `El dataset esta vacio.` |
| PF-05 | Columna incompleta en CSV | Una linea con menos de 17 columnas | Se lanza `Error de formato CSV en linea N: Se esperaban 17 columnas y se encontraron X.` |
| PF-06 | Valor no numerico en CSV | Una linea con texto donde se espera numero | Se lanza `Error de formato CSV en linea N: Uno o mas valores no son numericos.` |
| PF-07 | Opcion invalida del menu | Numero fuera del rango 0-9 | El programa muestra `Opcion no valida.` |
| PF-08 | Configuracion invalida de hilos | `0` o numero negativo | El programa muestra `El numero de hilos debe ser mayor que cero.` |
| PF-09 | Benchmark de estadisticas generales | Opcion 8 y analisis 1 | Se imprime primero el resultado de la operacion y despues la tabla con tiempo, hilos, speedup y eficiencia. |
| PF-10 | Benchmark de factores | Opcion 8 y analisis 7 | Se imprime la tabla de factores y la tabla resumen del benchmark. |

## Evidencia sugerida

- Captura de pantalla de la carga correcta con `small`.
- Captura de pantalla de la carga correcta con `medium`.
- Captura de pantalla de un error por archivo inexistente.
- Captura de pantalla de un CSV mal formado.
- Captura de pantalla del benchmark con al menos una configuracion de hilos.

## Notas para el reporte final

- Ejecutar y documentar en `Release x64`.
- Guardar los resultados de comparacion secuencial vs paralela.
- Incluir solo pruebas reproducibles con las rutas exactas de los archivos usados.
