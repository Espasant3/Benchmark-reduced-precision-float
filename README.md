# TFG: Operaciones con Float de 16 Bits

Este repositorio contiene los programas desarrollados como parte de mi Trabajo de Fin de Grado (TFG) sobre operaciones con números de punto flotante de 16 bits, en él se encuentran distintos programas en código C con varios algoritmos. Los programas cuentan con una versión que emplea el tipo de dato float de 32 bits y otras versiones que emplean distintos tipos de datos que son tratados como float de 16 bits.

## Tabla de contenidos
- [Contenido](#contenido)
- [Requisitos](#requisitos)
- [Información relevante](#información-relevante)
  - [Extensión de los ejecutables](#extensión-de-los-ejecutables)
  - [Nombres de los archivos](#nombres-de-los-archivos)
    - [Parámetros de los scripts](#parámetros-de-los-scripts)
  - [Arquitecturas y Vendors contemplados en los scripts compile_all.sh y run_all.sh](#arquitecturas-y-vendors-contemplados-en-los-scripts-compile_allsh-y-run_allsh)
- [Preparación del entorno](#preparación-del-entorno)
  - [Requisitos Previos](#requisitos-previos)
  - [Instalación de Intel SDE](#instalación-de-intel-sde)
  - [Instalación de QEMU](#instalación-de-qemu)
  - [Docker](#docker)
- [Uso](#uso)
- [Pruebas Python](#pruebas-python)
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
  - `<gsl/gsl_eigen.h>`: [Documentación](https://www.gnu.org/software/gsl/doc/html/eigen.html)
  - `<math.h>`: [Documentación](https://www.cplusplus.com/reference/cmath/)
  - `<arm_fp16.h>`: (para soporte del tipo de dato `__fp16`, float16, específico de ARM)
  - `<arm_bf16.h>`: (para soporte del tipo de dato `__bf16`, bfloat, específico de ARM)



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
| `<nombre>_compile_<arch>.sh`  | Script para compilar los programas `<nombre>` en el mismo directorio, para la arquitectura `<arch>`.             |
| `<nombre>_run_<arch>.sh`      | Script para ejecutar todos los programas compilados en el directorio actual, para la arquitectura `<arch>`.      |
| `compile_all.sh`       | Script general para compilar todos los programas, escogiendo los scripts de compilación adecuados para la arquitectura. |
| `run_all.sh`           | Script general para ejecutar todos los programas, escogiendo los scripts de compilación adecuados para la arquitectura. |


**Nota**: `<arch>` no se refiere solo a la arquitectura sino que es una combinación de la arquitectura, obtenida mediante el comando `uname -m`, y el proveedor de la CPU, obtenido mediante el comando `grep -m 1 'vendor_id' /proc/cpuinfo | awk '{print $3}'`.

#### Parámetros de los scripts

Los scripts de compilación y ejecución aceptan los siguientes parámetros y flags:

- **Parámetros**:
  - `<tamanho N>`: Tamaño del vector a emplear. Debe ser un número positivo mayor que 0.
  - `[<seed>]`: (Opcional) Semilla para la generación de números aleatorios. Debe ser un número positivo mayor que 0 si se proporciona.

- **Flags**:
  - `--force`: Permite la compilación cruzada y la ejecución de programas para una arquitecturas diferente a la actual (empleando Intel SDE y QEMU). En la compilación, este flag permite la cross-compilación de Intel/AMD a ARM. En la ejecución en Intel/AMD utiliza QEMU para ejecutar códigos ARM y en ARM emplea Intel SDE para ejecutar códigos Intel/AMD.

- **Scripts de compilación**:
  - `<nombre>_compile_<arch>.sh`: Compila los programas `<nombre>` para la arquitectura `<arch>`.
    - Uso: `./<nombre>_compile_<arch>.sh [--force]`

- **Scripts de ejecución**:
  - `<nombre>_run_<arch>.sh`: Ejecuta todos los programas compilados en el directorio actual para la arquitectura `<arch>`.
    - Uso: `./<nombre>_run_<arch>.sh <tamanho N> [<seed>] [--force]`

- **Scripts generales**:
  - `compile_all.sh`: Compila todos los programas, seleccionando los scripts de compilación adecuados para la arquitectura.
    - Uso: `./compile_all.sh [--force]`
  - `run_all.sh`: Ejecuta todos los programas, seleccionando los scripts de ejecución adecuados para la arquitectura.
    - Uso: `./run_all.sh <tamanho N> [<seed>] [--force]`

**Nota**: El flag `--force` en los scripts de compilación (excepto el de ARM) y ejecución fuerza la compilación cruzada y la ejecución de programas para arquitecturas ARM utilizando QEMU e Intel SDE.


### Arquitecturas y Vendors contemplados en los scripts `compile_all.sh` y `run_all.sh`

#### x86_64 (Intel y AMD)
- **Arquitectura**: x86_64
- **Vendors**:
  - **GenuineIntel**: Procesadores Intel de 64 bits.
    - `<arch>`: Intel 
  - **AuthenticAMD**: Procesadores AMD de 64 bits.
      - `<arch>`: AMD 

#### ARM aarch64 (genérica)
- **Arquitectura**: aarch64
- **Vendor**: Procesadores ARM de 64 bits genéricos, excluyendo aquellos fabricados por Nvidia y HiSilicon (Huawei).
    - `<arch>`: ARM 

#### Huawei aarch64
- **Arquitectura**: aarch64
- **Vendor**: HiSilicon (identificado como "huawei" en el script)
  - **Huawei Ascend**: Procesadores orientados a IA de Huawei.
  - **HiSilicon Kunpeng**: Procesadores ARM de servidor de Huawei.
- **Nota**: Por el momento no hay soporte oficial para Huawei (tanto Huawei Ascend como HiSilicon Kunpeng).

#### MIPS (Loongson y genérica)
- **Arquitectura**: mips, mips64
- **Vendors**:
  - **Loongson**: Procesadores basados en la arquitectura MIPS.
  - **MIPS**: Procesadores MIPS genéricos.
- **Nota**: No hay soporte oficial por el momento.

#### RISC-V (Loongson y genérica)
- **Arquitectura**: riscv, riscv64
- **Vendors**:
  - **Loongson**: Procesadores basados en la arquitectura RISC-V.
  - **RISC-V**: Procesadores RISC-V genéricos.
- **Nota**: No hay soporte oficial por el momento.

#### Power-ISA
- **Arquitectura**: powerpc, ppc64, ppc64le
- **Vendor**: Power-ISA
- **Nota**: No hay soporte oficial por el momento.

#### LoongArch
- **Arquitectura**: loongarch64
- **Vendor**: LoongArch
- **Nota**: No hay soporte oficial por el momento.

#### Nvidia
- **Arquitectura**: aarch64
- **Vendor**: Nvidia
- **Nota**: No hay soporte oficial por el momento.

**Nota**: Para más información sobre los valores de `uname -m`, consultar la [Documentación del proyecto](./Bibliografia/URLs.md#architecturespecificsmemo)

**Nota**: Los archivos ejecutados por el script `<nombre>_run_<arch>.sh` deben cumplir con los criterios especificados en la sección [Extensión de los ejecutables](#extensión-de-los-ejecutables).


## Preparación del entorno para Intel x86

### Requisitos Previos

Antes de comenzar, asegúrate de tener instalados los siguientes paquetes en tu sistema:

- **GCC** (GNU Compiler Collection) versión 14 o superior
- **Compiladores cruzados para ARM** (para compilar programas para ARM)
- **QEMU** para emulación de ARM (para emular la arquitectura ARM)
- **Intel SDE** (opcional, para emular instrucciones AVX512FP16)

Puedes instalar estos paquetes en sistemas basados en Debian/Ubuntu con el siguiente comando:

```bash
sudo apt update
sudo apt install gcc-14 qemu-user gcc-arm-linux-gnueabihf gcc-aarch64-linux-gnu -y
```

### Instalación de Intel SDE

1. Descarga la versión más reciente de Intel SDE (9.44) desde el [sitio oficial de Intel](https://www.intel.com/content/www/us/en/developer/articles/tool/software-development-emulator.html). 
2. Extrae el archivo descargado:
    ```bash
    tar -xzf sde-9.44.tar.gz
    ```
3. Navega al directorio extraído
    ```bash
    cd sde-9.44
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
    git clone git@github.com:Espasant3/TFG.git
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
    git clone git@github.com:Espasant3/TFG.git
    ```
2. Compilar el código:
    ```bash
    ./<nombre>_compile.sh
    ```
    En el futuro es probable que se añada un script en el directorio raíz que permita compilar todos los archivos a la vez.
    
3. Ejecutar el programa:
    ```bash
    ./programa-compilado <tamanho N> [<seed>] [--force]
    ```
    Opcionalmente se pueden comprobar fugas de memoria con:
    ```bash
    valgrind --leak-check=full ./programa-compilado <tamanho N> [<seed>] [--force]
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


## Contacto

Para cualquier duda o sugerencia, por favor contacta conmigo a través de [eloi.barcon@rai.usc.es](mailto:eloi.barcon@rai.usc.es).

---

Este trabajo es parte de mi TFG en [Universidade de Santiago de Compostela](https://www.usc.gal/gl), supervisado por [Pablo Quesada Barriuso](https://citius.gal/gl/team/pablo-quesada-barriuso/), como tutor, y por [Javier López Fandiño](https://citius.gal/gl/team/javier-lopez-fandino/), como co-tutor.

