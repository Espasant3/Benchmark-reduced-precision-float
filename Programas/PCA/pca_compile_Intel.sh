#!/bin/bash

### SCRIPT DE COMPILACION PARA ARQUITECTURA INTEL x86

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

COMMON_FLAGS="-Wall -g"

OPT_FLAGS="-march=icelake-client -mtune=icelake-client -O3 -fomit-frame-pointer -fPIC"

LINK_FLAGS="-lm -llapacke -llapack -lblas"

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

### COMPILACION DEL PROGRAMA BASE

gcc-14 $COMMON_FLAGS pca_FP32.c -o pca_FP32 $OPT_FLAGS $LINK_FLAGS

