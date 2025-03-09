#!/bin/bash

# Inicializar variables
force_flag=""
additional_flags=""

# Verificar si el primer argumento es --force
if [[ "$1" == "--force" ]]; then
    force_flag="--force"
    shift
else
    # Procesar argumentos de entrada
    for arg in "$@"; do
        if [[ "$arg" == "--force" ]]; then
            force_flag="--force"
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

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

# This script checks the CPU architecture and vendor.
# The values for `uname -m` can be found at: https://wiki.debian.org/ArchitectureSpecificsMemo

# Obtener la arquitectura del sistema
ARCH=$(uname -m)

# Obtener el proveedor del CPU
VENDOR=$(grep -m 1 'vendor_id' /proc/cpuinfo | awk '{print $3}')

# Dependiendo del proveedor del CPU, realizar diferentes acciones
case "$VENDOR" in
    GenuineIntel)
        CPU_VENDOR="Intel"
        ;;
    AuthenticAMD)
        CPU_VENDOR="AMD"
        ;;
    HiSilicon)
        CPU_VENDOR="Huawei"
        ;;
    *)
        CPU_VENDOR="unknown"
        ;;
esac

# Directorios a procesar (se puede expandir fácilmente)
DIRECTORIOS=("AXPY" "DCT" "DWT_1D" "PCA")

# Dependiendo de la combinación de arquitectura y proveedor, realizar diferentes acciones
case "$ARCH" in
    x86_64|amd64|x64)

        echo "Arquitectura: $CPU_VENDOR x86_64"
        for DIR in "${DIRECTORIOS[@]}"; do

            dir_lower=$(echo "$DIR" | tr '[:upper:]' '[:lower:]')  # Conversión a minúsculas
            script="${DIR}/${dir_lower}_compile_${CPU_VENDOR}.sh"  # Formato: directorio/dir_compile_ARCH.sh
            if [[ -f "$script" && -x "$script" ]]; then
                echo "Compilando $DIR"
                ./"$script" $force_flag $additional_flags
            else
                echo "Error: Script no encontrado o no ejecutable -> $script"
            fi

        done

        ;;
    aarch64|arm64)

        echo "Arquitectura: $CPU_VENDOR ARM de 64 bits"

        if [[ "$CPU_VENDOR" == "Huawei" ]]; then
            echo "Arquitectura: Huawei de 64 bits"
            echo "No hay soporte oficial por el momento."
        else
            CPU_VENDOR="ARM"
        fi

        for DIR in "${DIRECTORIOS[@]}"; do

            dir_lower=$(echo "$DIR" | tr '[:upper:]' '[:lower:]')  # Conversión a minúsculas
            script="${DIR}/${dir_lower}_compile_${CPU_VENDOR}.sh"  # Formato: directorio/dir_compile_ARCH.sh
            if [[ -f "$script" && -x "$script" ]]; then
                echo "Compilando $DIR"
                ./"$script" $force_flag $additional_flags
            else
                echo "Error: Script no encontrado o no ejecutable -> $script"
            fi

        done

        ;;
    *)
        echo "Arquitectura desconocida o no soportada: $ARCH"
        ;;
esac

exit 0