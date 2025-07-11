# TFG: Operaciones con Float de 16 Bits

Este repositorio contiene los programas desarrollados como parte de mi Trabajo de Fin de Grado (TFG) sobre operaciones con números de punto flotante de 16 bits, en él se encuentran distintos programas en código C con varios algoritmos. Los programas cuentan con una versión que emplea el tipo de dato float de 32 bits y otras versiones que emplean distintos tipos de datos que son tratados como float de 16 bits.

## Tabla de contenidos
- [Contenido](#contenido)
- [Requisitos](#requisitos)
- [Información relevante](#información-relevante)
  - [Extensión de los ejecutables](#extensión-de-los-ejecutables)
  - [Nombres de los archivos](#nombres-de-los-archivos)
    - [Parámetros de los scripts](#parámetros-de-los-scripts)
      - [Script de compilación](#script-de-compilación)
      - [Script de ejecución](#script-de-ejecución)
  - [Arquitecturas y Vendors contemplados en los scripts compile_all.sh y run_all.sh](#arquitecturas-y-vendors-contemplados-en-los-scripts-compile_allsh-y-run_allsh)
- [Preparación del entorno](#preparación-del-entorno)
  - [Requisitos Previos](#requisitos-previos)
  - [Instalación de Intel SDE](#instalación-de-intel-sde)
  - [Instalación de QEMU](#instalación-de-qemu)
  - [Docker](#docker)
- [Uso](#uso)
- [Pruebas Python](#pruebas-python)
- [Licencias](#licencias)
- [Contacto](#contacto)



## Contenido

- **Introducción**: Descripción del proyecto y su relevancia.
- **Implementación**: Explicación detallada del código y las técnicas utilizadas.
- **Pruebas**: Métodos de prueba y resultados obtenidos.
- **Conclusiones**: Resumen de los hallazgos y posibles futuras mejoras.

## Requisitos

- **Compilador**: [GCC 14.2 ](https://gcc.gnu.org/onlinedocs/14.2.0/)
- **Librerías**: 
  - `<time.h>`: [Documentación](https://www.cplusplus.com/reference/ctime/)
  - `<math.h>`: [Documentación](https://www.cplusplus.com/reference/cmath/)
  - `<cblas.h>`:  [Documentación](https://www.gnu.org/software/gsl/doc/html/cblas.html)
  - `<lapacke.h>`: [Documentación](https://netlib.org/lapack/lapacke.html)
  - `<arm_fp16.h>`: (para soporte del tipo de dato `__fp16`, float16, específico de ARM)
  - `<arm_bf16.h>`: (para soporte del tipo de dato `__bf16`, bfloat16, cuando se compila para ARM)
  - `<armpl.h>`: [Documentación](https://developer.arm.com/documentation/101004/2410/General-information/Arm-Performance-Libraries)



## Información relevante

### Extensión de los ejecutables

Los scripts están configurados para manejar exclusivamente las siguientes extensiones y situaciones específicas:

- **x86 (32/64 bits)**: sin extensión
- **ARM (32 bits)**: `.o`
- **ARM (64 bits)**: `.out`
 
**Nota**: No existen scripts para la compilación para ARM de 32 bits, ya que los tipos de datos (`__fp16` y `__bf16`) no son compatibles de manera nativa. Sin embargo, se pueden encontrar instrucciones sobre compilación cruzada y cómo verificar la ejecución con QEMU para esta arquitectura en la [sección de uso](#uso).

### Nombres de los archivos

Los programas tienen el siguiente formato de nombres, donde `<nombre>` corresponde al nombre del algoritmo específico (axpy, dct, dwt_1d o pca):

| Nombre del archivo     | Descripción                                                                                                             |
|------------------------|-------------------------------------------------------------------------------------------------------------------------|
| `<nombre>_FP32.c`      | Versión original del programa que utiliza floats de 32 bits.                                                            |
| `<nombre>_FP16.c`      | Versión del programa que utiliza el tipo de dato `_Float16`.                                                            |
| `<nombre>_FP16_ARM.c`  | Versión del programa que utiliza el tipo de dato `__fp16` específico de ARM.                                            |
| `<nombre>_BF16.c`      | Versión del programa que utiliza el tipo de dato `__bf16` específico de ARM.                                            |
| `<nombre>_compile_<target>.sh`  | Script para compilar los programas `<nombre>` en el mismo directorio, para la arquitectura `<target>`.             |
| `<nombre>_run_<target>.sh`      | Script para ejecutar todos los programas compilados en el directorio actual, para la arquitectura `<target>`.      |
| `compile_all.sh`       | Script general para compilar todos los programas, escogiendo los scripts de compilación adecuados para la arquitectura. |
| `run_all.sh`           | Script general para ejecutar todos los programas, escogiendo los scripts de compilación adecuados para la arquitectura. |


**Nota**: `<target>` no se refiere solo a la arquitectura sino que es una combinación de la arquitectura, obtenida mediante el comando `uname -m`, y el proveedor de la CPU, obtenido mediante el comando `grep -m 1 'vendor_id' /proc/cpuinfo | awk '{print $3}'`.

#### Parámetros de los scripts

#### Script de compilación
Los scripts de compilación aceptan los siguientes parámetros y flags:

- **Parámetros**:
  - `[flags]`: (Opcional) Flags para modificar el comportamiento del script. Estos flags se añadirán al comando de compilación.

- **Flags**:
  - `-f` o `--force`: Permite la compilación cruzada de programas para una arquitecturas diferente a la actual. Este flag solo permite la cross-compilación de Intel/AMD a ARM (genera programas con extensión `.out`), por lo que en la compilación en ARM no tendrá efecto. 

- **Scripts de compilación**:
  - `<nombre>_compile_<target>.sh`: Compila los programas `<nombre>` para la arquitectura `<target>`.
    - Uso: `./<nombre>_compile_<target>.sh [-f|--force] [flags]`
  - `compile_all.sh`: Script general que compila todos los programas, seleccionando los scripts de compilación adecuados para la arquitectura.
    - Uso: `./compile_all.sh [-f|--force] [flags]`


#### Script de ejecución
Los scripts de ejecución aceptan los siguientes parámetros y flags:

- **Parámetros**:
  - `<tamanho N>`: Tamaño del vector a emplear. Debe ser un número positivo mayor que 0.
  - `[<seed>]`: (Opcional) Semilla para la generación de números aleatorios. Debe ser un número positivo mayor que 0 si se proporciona.

- **Flags**:
  - `-f` o `--force`: Permite la ejecución de programas para una arquitecturas diferente a la actual (empleando Intel SDE y QEMU). Durante la ejecución en Intel/AMD utiliza QEMU para ejecutar códigos ARM (programas con extensión `.out`) y en ARM emplea Intel SDE para ejecutar códigos Intel/AMD (programas sin extensión).
  - `-v` o `--verbose`: Muestra información detallada durante la ejecución del programa, que comprende los datos iniciales de ejecución completos y los resultados de la ejecución completos.

- **Scripts de ejecución**:
  - `<nombre>_run_<target>.sh`: Ejecuta todos los programas compilados en el directorio actual para la arquitectura `<target>`.
    - Uso: `./<nombre>_run_<target>.sh <tamanho N> [<seed>] [-f|--force] [-v|--verbose]`
  - `run_all.sh`: Script general que ejecuta todos los programas, seleccionando los scripts de ejecución adecuados para la arquitectura.
    - Uso: `./run_all.sh <tamanho N> [<seed>] [-f|--force] [-v|--verbose]`


### Arquitecturas y Vendors contemplados en los scripts `compile_all.sh` y `run_all.sh`

#### x86_64 (Intel y AMD)
- **Arquitectura**: x86_64
- **Vendors**:
  - **GenuineIntel**: Procesadores Intel de 64 bits.
    - `<target>`: Intel 
  - **AuthenticAMD**: Procesadores AMD de 64 bits.
      - `<target>`: AMD 

#### ARM aarch64 (genérica)
- **Arquitectura**: aarch64
- **Vendor**: Procesadores ARM de 64 bits genéricos, excluyendo aquellos fabricados por Nvidia y HiSilicon (Huawei).
    - `<target>`: ARM 

#### Huawei aarch64
- **Arquitectura**: aarch64
- **Vendor**: HiSilicon (identificado como "huawei" en el script)
  - **Huawei Ascend**: Procesadores orientados a IA de Huawei.
  - **HiSilicon Kunpeng**: Procesadores ARM de servidor de Huawei.
- **Nota**: Finalmente fue descartado pero está considerado en los scripts globales (tanto Huawei Ascend como HiSilicon Kunpeng).

**Nota**: Para más información sobre los valores de `uname -m`, consultar la [Documentación del proyecto](./Bibliografia/URLs.md#architecturespecificsmemo)

**Nota**: Los archivos ejecutados por los scripts `<nombre>_run_<target>.sh` deben cumplir con los criterios especificados en la sección [Extensión de los ejecutables](#extensión-de-los-ejecutables) para poder ser ejecutados en los scripts.


## Preparación del entorno para Intel x86

### Requisitos Previos

Antes de comenzar, asegúrate de tener instalados los siguientes paquetes en tu sistema:

- **GCC** (GNU Compiler Collection) versión 14 o superior
- **Compiladores cruzados para ARM** (para compilar programas para ARM)
- **QEMU** para emulación de ARM (para emular la arquitectura ARM)
- **Intel SDE** (opcional, para emular instrucciones AVX512FP16 y AVX512BF16 en Intel/AMD)

Puedes instalar estos paquetes en sistemas basados en Debian/Ubuntu con el siguiente comando:

```bash
sudo apt update
sudo apt install gcc-14 qemu-user gcc-14-aarch64-linux-gnu -y
```

### Instalación de Intel SDE

1. Descarga la versión más reciente de Intel SDE (9.53 en este momento) desde el [sitio oficial de Intel](https://www.intel.com/content/www/us/en/download/684897/intel-software-development-emulator.html). 
2. Extrae el archivo descargado:
    ```bash
    tar -xzf sde-9.53.tar.gz
    ```
3. Navega al directorio extraído
    ```bash
    cd sde-9.53
    ```
4. (Opcional) Puedes mover el ejecutable `sde` a un directorio en tu `PATH` para facilitar su uso: 
    ```bash
    sudo mv sde /usr/local/bin/
    ```

### Instalación de QEMU
- Si tu sistema no es compatible con la arquitectura ARM de 64 bits, asegúrate de tener `qemu-aarch64` instalado para poder ejecutar los programas compilados para ARM
- Puedes verificar que `qemu-aarch64` está instalado e instalar en caso de que no lo esté con:
  ```bash
  command -v qemu-aarch64 || sudo apt install qemu-user
  ```

### Docker

Opcionalmente se dispone de un fichero Dockerfile en el directorio raíz del proyecto para la configuración del entorno en un contenedor docker. 

Pasos para montar el contenedor:
1. Clonar el repositorio:
    ```bash
    git clone git@github.com:Espasant3/Benchmark-reduced-precision-float.git
    ```
2. Navegar al directorio raíz del proyecto:
    ```bash
    cd project_root_directory
    ```
3. Construye la imagen docker
    ```bash
    docker build -t tfg-benchmarck-dev-env .
    ```
4. Arrancar el contenedor
    ```bash
    docker run -it --rm -v $(pwd):/workspace tfg-benchmarck-dev-env
    ```

**Nota**: Asegúrate de estar en el directorio raíz del proyecto para que se monte adecuadamente el contenedor Docker. 

## Uso

1. Clonar el repositorio:
    ```bash
    git clone git@github.com:Espasant3/Benchmark-reduced-precision-float.git
    ```
2. Compilar el código:
    ```bash
    ./<nombre>_compile.sh
    ```
    En el futuro es probable que se añada un script en el directorio raíz que permita compilar todos los archivos a la vez.
    
3. Ejecutar el programa:
    ```bash
    ./programa-compilado <tamanho N> [<seed>]
    ```
    Opcionalmente se pueden comprobar fugas de memoria con:
    ```bash
    valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./programa-compilado <tamanho N> [<seed>]
    ```
4. Cross-compilar el código para ARM:
   
   Para ARM de 32 bits:
   ```bash
   arm-linux-gnueabihf-gcc nombre-del-programa.c -o programa-compilado.o -Wall
   ```
   Para ARM de 64 bits:
   ```bash
   aarch64-linux-gnu-gcc nombre-del-programa.c -o programa-compilado.out -Wall
   ```
5. Ejecutar el programa emulando ARM:

   Para ARM de 32 bits:
   ```bash
   qemu-arm ./programa-compilado_arm32
   ```
   Para ARM de 64 bits:
   ```bash
   qemu-aarch64 ./programa-compilado_arm64
   ```


## Pruebas Python

En el directorio Tests-Python se encuentran programas que se pueden emplear para comparar el funcionamiento de los programas originales (con los que comparten prefijo).

Para gestionar el entorno virtual de python se pueden utilizar los siguientes ficheros:
1. Script `setup-python.sh`: script encargado de crear el entorno virtual en el propio directorio en el que se encuentra y de instalar todos los paquetes de python necesarios.
2. Fichero `requirements.txt`: fichero que tiene todos los paquetes que se emplean en los programas y que son instalados durante la ejecución del script anterior.

**Aviso:** debido a las diferencias de precisión y lógica interna de C y Python los resultados pueden no ser iguales pese a tener la misma semilla y realizar las mismas operaciones.


## Licencias

- Este proyecto y los archivos en el directorio `Programas/PCA_REIMPL/functions-adapted/include`: **Licencia MIT**  
  Ver [LICENSE](LICENSE)  
  Copyright: © 2025 Eloi Barcón Piñeiro

- Código adaptado de LAPACK (que está en el directorio `Programas/PCA_REIMPL/functions-adapted/`): **Licencia BSD modificada**  
  Ver [LICENSE_LAPACK](LICENSE_LAPACK)  
  Copyright original:  
  © 1992-2025 University of Tennessee, University of California Berkeley,  
  University of Colorado Denver, y colaboradores.

## Licencias

### **Código original**
1. **LICENCIA MIT**: 
  - Archivos: todos los archivos en el directorio `Programas/PCA_REIMPL/functions-adapted/include` y el resto de programas del proyecto.
  - Copyright: © 2025 Eloi Barcón Piñeiro
  - Detalles: [LICENSE](LICENSE)  

### **Código derivado de terceros**
1. **LAPACK (BSD modificada)**:  
   - Archivos: 
     - `Programas/PCA_REIMPL/functions-adapted/fortran_sourced/*`
     - `Programas/PCA_REIMPL/functions-adapted/utils/c_xerbla.c`
     - `Programas/PCA_REIMPL/functions-adapted/utils/hfroundup_lwork.c`
     - `Programas/PCA_REIMPL/functions-adapted/utils/ieeck_reimpl_half_precision.c`
     - `Programas/PCA_REIMPL/functions-adapted/utils/iparmq_reimpl.c`
     - `Programas/PCA_REIMPL/functions-adapted/utils/lsame_reimpl.c`
   - Copyright original:  
     © 1992-2025 University of Tennessee, University of California Berkeley, University of Colorado Denver, y colaboradores. 
   - Detalles: [LICENSE_LAPACK](LICENSE_LAPACK)

2. **Intel LAPACKE (BSD)**:  
   - **Archivos con licencia 2010**:  
     - `Programas/PCA_REIMPL/functions-adapted/utils/lapacke_xerbla.c`  
     - Copyright: © 2010 Intel Corporation  
   - **Archivos con licencia 2014**:  
     - `Programas/PCA_REIMPL/functions-adapted/utils/hfge_trans.c`  
     - `Programas/PCA_REIMPL/functions-adapted/utils/hfsy_nancheck.c`  
     - `Programas/PCA_REIMPL/functions-adapted/utils/hfsy_trans.c`  
     - `Programas/PCA_REIMPL/functions-adapted/utils/hftr_nancheck.c`  
     - `Programas/PCA_REIMPL/functions-adapted/utils/hftr_trans.c`  
     - `Programas/PCA_REIMPL/functions-adapted/src/lapacke_hfsyev*.c`  
     - Copyright: © 2014 Intel Corporation  
   - **Archivos con licencia 2017**:  
     - `Programas/PCA_REIMPL/functions-adapted/utils/lapacke_nancheck_reimpl.c`  
     - Copyright: © 2017 Intel Corporation  
   - **Detalles**: Los avisos de licencia BSD de Intel están incluidos en cada archivo.


## Contacto

Para cualquier duda o sugerencia, por favor contacta conmigo a través de [eloi.barcon@rai.usc.es](mailto:eloi.barcon@rai.usc.es).

---

Este trabajo es parte de mi TFG en [Universidade de Santiago de Compostela](https://www.usc.gal/gl), supervisado por [Pablo Quesada Barriuso](https://citius.gal/gl/team/pablo-quesada-barriuso/), como tutor, y por [Javier López Fandiño](https://citius.gal/gl/team/javier-lopez-fandino/), como co-tutor.

