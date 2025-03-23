#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA INTEL x86

# Inicializar variables
force_run=false
additional_flags=""

# Uso: $0 [--force] [opciones adicionales]
usage() {
    echo "Uso: $0 [-f|--force] [opciones adicionales]"
    exit 1
}

# Procesar argumentos manualmente
while [[ $# -gt 0 ]]; do
    case "$1" in
        -f|--force)
            force_run=true
            shift
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

COMMON_FLAGS="-Wall -g"

OPT_FLAGS="-march=icelake-client -mtune=icelake-client -O3 -fomit-frame-pointer -fPIC $additional_flags"

LINK_FLAGS="-lm -llapacke -llapack -lblas"

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"


### COMPILACION DEL PROGRAMA BASE

gcc-14 $COMMON_FLAGS pca_FP32.c -o pca_FP32 $OPT_FLAGS $LINK_FLAGS

### COMPILACION DEL PROGRAMA DE CON FLOAT DE 16 BITS QUE EMPLEA EL TIPO DE DATO _Float16

if grep -q "sse2" /proc/cpuinfo; then
    echo "SSE2 support detected. Compiling programs with _Float16 data."

    gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16 -fexcess-precision=16 $OPT_FLAGS $LINK_FLAGS

    if grep -q "avx512fp16" /proc/cpuinfo; then
        
        echo "AVX512FP16 support detected. Compiling with -mavx512fp16."
        COMMON_FLAGS+=" -mavx512fp16"

        # Compilar el programa optimizado con AVX512 (nativo)
        gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16_native-base $OPT_FLAGS $LINK_FLAGS
        # Compilar el programa optimizado con AVX512 y precisión de 16 bits
        gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16_avx512_precision -fexcess-precision=16 $OPT_FLAGS $LINK_FLAGS
        # Compilar el programa con máxima optimización
        gcc-14 $COMMON_FLAGS pca_FP16.c -o pca_FP16_max_performance -fexcess-precision=16 -mfpmath=sse $OPT_FLAGS $LINK_FLAGS

        # Eliminar los flags específicos de esta sección
        COMMON_FLAGS="${COMMON_FLAGS/-mavx512fp16/}" 
        COMMON_FLAGS=$(echo $COMMON_FLAGS | tr -s ' ' | xargs)

    else
        echo "AVX512FP16 not supported on this system. Skipping compilation with -mavx512fp16."
    fi

else
    echo "SSE2 not supported on this system. Skipping compilation for programs with _Float16."
fi

# Compilación cruzada para ARM de 64 bits

if $force_run; then

    echo "Flag [-f]--force detectada. Cross-compilando programas para arquitectura ARM."

    echo "No es posible compilar para ARM los programas de PCA debido a la necesidad de librerías específicas de arquitecturas ARM."

fi

exit 0