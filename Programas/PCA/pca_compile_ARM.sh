#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 64 BITS

# Inicializar variables
force_run=false
additional_flags=""

# Uso: $0 [--force] [opciones adicionales]
usage() {            
    # Mostrar ayuda de uso del script
    echo "Uso: $0 [-f|--force] [opciones adicionales]"
    echo "  -f, --force       Fuerza la compilación cruzada de todos los programas a la arquitectura aarch64."
    echo "  -h, --help        Muestra esta ayuda y sale."
    exit 0
}

# Procesar argumentos manualmente
while [[ $# -gt 0 ]]; do
    case "$1" in
        -f|--force)
            force_run=true
            shift
            ;;
        -h|--help)
            usage
            ;;
        --)  # Fin de las opciones
            shift
            break
            ;;
        -*)
            # Flags adicionales para el compilador
            additional_flags+=" $1"
            echo "Flag adicional añadido para compilar: $1"
            shift
            ;;
        *)
            # Argumentos posicionales (tamaño N, seed, etc.)
            break
            ;;
    esac
done

COMMON_FLAGS="-Wall"

OPT_FLAGS="-O3 -march=armv8.2-a+fp16+fp16fml+simd -ftree-vectorize -fomit-frame-pointer -fPIC $additional_flags"

LINK_FLAGS="-lm -llapacke -llapack -lblas"

# Definir rutas predeterminadas para la librería armpl (pueden ser sobrescritas por el usuario)
ARMPL_LIB="/opt/arm/armpl_24.10_gcc/lib/"
ARMPL_INCLUDE="/opt/arm/armpl_24.10_gcc/include/"

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"


### COMPILACION DEL PROGRAMA BASE

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS pca_FP32.c -o pca_FP32.out $OPT_FLAGS $LINK_FLAGS


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

LINK_FLAGS+=" -larmpl -L${ARMPL_LIB} -I${ARMPL_INCLUDE}"

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS -fexcess-precision=16 pca_FP16.c -o pca_FP16.out $OPT_FLAGS $LINK_FLAGS

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __fp16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS pca_FP16_ARM.c -o pca_FP16_ARM.out $OPT_FLAGS $LINK_FLAGS


### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS PARA ARQUITECTURA ARM (EMPLEA EL TIPO DE DATO __bf16)

# Compila para ARM de 64 bits, como distintivo el archivo tiene la extension .out
gcc-14 $COMMON_FLAGS pca_BF16.c -o pca_BF16.out $OPT_FLAGS $LINK_FLAGS

# Eliminar los flags específicos de esta sección 
LINK_FLAGS="${LINK_FLAGS/-larmpl/}"
LINK_FLAGS="${LINK_FLAGS/-L${ARMPL_LIB}/}"
LINK_FLAGS="${LINK_FLAGS/-I${ARMPL_INCLUDE}/}"
LINK_FLAGS=$(echo $LINK_FLAGS | tr -s ' ' | xargs)

exit 0
