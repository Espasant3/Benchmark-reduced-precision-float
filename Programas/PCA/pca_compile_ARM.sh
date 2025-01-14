#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA ARM DE 64 BITS

# Inicializar variables
force_run=false
additional_flags=""

# Verificar si el primer argumento es --force
if [[ "$1" == "--force" ]]; then
    force_run=true
    shift
else
    # Procesar argumentos de entrada
    for arg in "$@"; do
        if [[ "$arg" == "--force" ]]; then
            force_run=true
            set -- "${@/--force/}"
            break
        fi
    done
fi

# Procesar flags adicionales
for arg in "$@"; do
    if [[ "$arg" == -* && "$arg" != --* ]]; then
        additional_flags+=" $arg"
    else
        echo "Flag no válida: $arg"
        exit 1
    fi
done

COMMON_FLAGS="-Wall"

OPT_FLAGS="-O3 -march=armv8.2-a+fp16+fp16fml+simd -ftree-vectorize -fomit-frame-pointer -fPIC $additional_flags"

LINK_FLAGS="-lm -llapacke -llapack -lblas"

# Definir rutas predeterminadas para la librería armpl (pueden ser sobrescritas por el usuario)
ARMPL_LIB="$HOME/armpl/lib"
ARMPL_INCLUDE="$HOME/armpl/include"

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
