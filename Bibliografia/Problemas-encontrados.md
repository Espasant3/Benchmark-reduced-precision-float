## Problemas Encontrados

1. La generación directa de datos aleatorios para tipos de punto flotante de media precisión resulta en valores NaN. Es necesario utilizar una variable intermedia y luego realizar un cast para generar los datos correctamente.
2. Mi procesador no soporta instrucciones AVX512FP16 de manera nativa.
3. En el código del algoritmo de PCA, como trabajamos con cblas y lapacke, las versiones que emplean float de media precision (y brain float) no son completamente igual a la versión que emplea float de 32 bits porque no se hacen exactamente las mismas operaciones. 
 - La función `cblas_sgemm` tiene una función equivalente llamada `cblas_hgemm`, definida en la libería armpl.h, que solo se puede utilizar en arquitecturas ARM y será utilizada cuando se trabaje en esta arquitectura (los programas que usen los tipos de dato _Float16 y __bf16 hará una conversión intermedia para poder utilizar esta función).
 - La función `LAPACKE_ssyev` por el contrario no dispone a día de hoy de ningún equivalente que trabaje con float de media precisión (o con brain float) por lo que todos los programas que no trabajan con el tipo de dato float tienen que hacer una conversión para poder trabajar con esta función.
4. No es posible solucionar el problema anterior realizando un cast al tipo de dato requerido por las funciones en la propia llamada a la función puesto que estas funciones requieren que la memoria esté alineada para funcionar correctamente.
