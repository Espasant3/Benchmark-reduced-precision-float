# TFG: Operaciones con Float de 16 Bits

Este repositorio contiene los programas desarrollados como parte de mi Trabajo de Fin de Grado (TFG) sobre operaciones con números de punto flotante de 16 bits, en él se encuentran distintos programas en código C con varios algoritmos. Los programas cuentan con una versión que emplea el tipo de dato float de 32 bits y otras versiones que emplean distintos tipos de datos que son tratados como float de 16 bits.

## Tabla de contenidos
- [Contenido](#contenido)
- [Requisitos](#requisitos)
- [Información relevante](#información-relevante)
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

- **Compilador**: [GCC 14.2 o superior](https://gcc.gnu.org/)
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


### Nombres de los archivos

Los programas tienen el siguiente formato de nombres, donde `<nombre>` corresponde al nombre del algoritmo específico (axpy, dct, dwt_1d o pca):

| Nombre del archivo     | Descripción                                                                 |
|------------------------|-----------------------------------------------------------------------------|
| `<nombre>_FP32.c`      | Versión original del programa que usa floats de 32 bits.                    |
| `<nombre>_FP16.c`      | Versión del programa que usa el tipo de dato `_Float16`.                    |
| `<nombre>_FP16_ARM.c`  | Versión del programa que usa el tipo de dato `__fp16` específico de ARM.    |
| `<nombre>_BF16.c`      | Versión del programa que usa el tipo de dato `__bf16` específico de ARM.    |
| `<nombre>_compile.sh`  | Script para compilar los programas `<nombre>` en el mismo directorio.       |
| `<nombre>_run.sh`      | Script para ejecutar todos los programas compilados en el directorio actual.|

Todas las versiones del programa que no usan floats de 32 bits utilizan un tipo de float de 16 bits.

**Nota**: Los archivos ejecutados con el script `<nombre>_run.sh` cumplen con los criterios especificados en la sección [Extensión de los ejecutables](#extensión-de-los-ejecutables).



## Preparación del entorno

### Requisitos Previos

Antes de comenzar, asegúrate de tener instalados los siguientes paquetes en tu sistema:

- **GCC** (GNU Compiler Collection) versión 14 o superior
- **Compiladores cruzados para ARM** (para compilar programas para ARM)
- **QEMU** para emulación de ARM (opcional en caso de estar ya en ARM, para emular la arquitectura ARM)
- **Intel SDE** (opcional, para emular instrucciones AVX512FP16)

Puedes instalar estos paquetes en sistemas basados en Debian/Ubuntu con el siguiente comando:

```bash
sudo apt update
sudo apt install gcc-14 qemu-user gcc-arm-none-eabi gcc-arm-linux-gnueabihf gcc-aarch64-linux-gnu -y
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
    docker run --rm -it tfg-benchmarck-dev-env
    ```


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
    ./programa-compilado <tamanho del vector> [<semilla>]
    ```
    Opcionalmente se pueden comprobar fugas de memoria con:
    ```bash
    valgrind --leak-check=full ./programa-compilado <tamanho del vector> [<semilla>]
    ```
4. Cross-compilar el código para ARM:
   
   Para ARM de 32 bits:
   ```bash
   arm-linux-gnueabihf-gcc nombre-del-programa.c -o programa-compialdo.o -Wall
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

