## Problemas Encontrados

1. La generación directa de datos aleatorios para tipos de punto flotante de media precisión resulta en valores NaN. Es necesario utilizar una variable intermedia y luego realizar un cast para generar los datos correctamente.
2. Los programas que emplean los tipos de datos `__fp16` o `__bf16` no pueden utilizar funciones con estos tipos de datos, ya sea como parámetros o valores de retorno.
3. Mi procesador no soporta instrucciones AVX512FP16 de manera nativa.

