# TFG: Operaciones con Float de 16 Bits

Este repositorio contiene los programas desarrollados como parte de mi Trabajo de Fin de Grado (TFG) sobre operaciones con números de punto flotante de 16 bits.

## Contenido

- **Introducción**: Descripción del proyecto y su relevancia.
- **Implementación**: Explicación detallada del código y las técnicas utilizadas.
- **Pruebas**: Métodos de prueba y resultados obtenidos.
- **Conclusiones**: Resumen de los hallazgos y posibles futuras mejoras.

## Requisitos

- **Compilador**: GCC 14.2 o superior
- **Librerías**: [LibraryName] (opcional) (Aún por determinar)
- **Librerías**: 
  - `<time.h>`
  - `<gsl/gsl_eigen.h>`
  - `<math.h>`
  - `<arm_fp16.h>` (para soporte del tipo de dato `__fp16`, float16, específico de ARM)
  - `<arm_bf16.h>` (para soporte del tipo de dato `__bf16`, bfloat, específico de ARM)


## Información relevante

- **Extensión de los ejecutables**:
  - **x86 (32/64 bits)**: sin extensión
  - **ARM (32 bits)**: `.o`
  - **ARM (64 bits)**: `.out`


## Uso

1. Clonar el repositorio:
    ```bash
    git clone https://github.com/Espasant3/TFG.git
    ```
2. Compilar el código:
    ```bash
    ./directorio-del-programa_compile.sh
    ```
    En el futuro es probable que se añada un script en el directorio raíz que permita compilar todos los archivos a la vez.
    
3. Ejecutar el programa:
    ```bash
    ./nombre-del-programa <tamaño del vector> [<semilla>]
    ```
    Opcionalmente se pueden comprobar fugas de memoria con:
    ```bash
    valgrind --leak-check=full ./nombre-del-programa <tamaño del vector> [<semilla>]
    ```
4. Cross-compilar el código para ARM:
   
   Para ARM de 32 bits:
   ```bash
   arm-linux-gnueabihf-gcc nombre-del-programa.c -o nombre-del-programa.o -Wall
   ```
   Para ARM de 64 bits:
   ```bash
   aarch64-linux-gnu-gcc nombre-del-programa.c -o nombre-del-programa.out -Wall
   ```
5. Ejecutar el programa emulando ARM:

   Para ARM de 32 bits:
   ```bash
   qemu-arm ./programa_arm32
   ```
   Para ARM de 64 bits:
   ```bash
   qemu-aarch64 ./programa_arm64
   ```


## Pruebas Python

En el directorio Tests-Python se encuentran programas que se pueden emplear para comparar el funcionamiento de los programas originales (con los que comparten prefijo).

Para gestionar el entorno virtual de python se pueden utilizar los siguientes ficheros:
1. Script `setup-python.sh`: script encargado de crear el entorno virtual en el propio directorio en el que se encuentra y de instalar todos los paquetes de python necesarios.
2. Fichero `requirements.txt`: fichero que tiene todos los paquetes que se emplean en los programas y que son instalados durante la ejecución del script anterior.

**Aviso:** debido a las diferencias de precisión y lógica interna de C y Python los resultados pueden no ser iguales pese a tener la misma semilla y realizar las mismas operaciones.


## Contacto

Para cualquier duda o sugerencia, por favor contacta conmigo a través de [eloi.barcon@rai.usc.es](mailto:eloi.barcon@rai.usc.es).

---

Este trabajo es parte de mi TFG en [Universidade de Santiago de Compostela](https://www.usc.gal/gl), supervisado por [Pablo Quesada Barriuso](https://citius.gal/gl/team/pablo-quesada-barriuso/), como tutor, y por [Javier López Fandiño](https://citius.gal/gl/team/javier-lopez-fandino/), como co-tutor.

