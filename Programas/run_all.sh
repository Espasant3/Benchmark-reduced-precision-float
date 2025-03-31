#!/bin/bash

# Inicializar variables
force_flag=""
verbose_flag=""

tamanhoN=""
seed=""

# Uso: $0 [-f|--force] [-v] <tamanho N> [<seed>]
usage() {
    echo "Uso: $0 [-f|--force] [-v] <tamanho N> [<seed>]"
    exit 1
}

# Procesar argumentos con GNU getopt
TEMP=$(getopt -o fvh --long force,help -n "$0" -- "$@")

# Verificar si hubo error en getopt
if [ $? != 0 ]; then
    echo "Error: Opción no reconocida o falta de argumento."
    usage
fi

eval set -- "$TEMP"

# Asignar variables basadas en opciones
while true; do
    case "$1" in
        -f|--force)
            force_flag="-f"
            shift
            ;;
        -v)
            verbose_flag="-v"
            shift
            ;;
        -h|--help)
            # Mostrar ayuda
            echo "Uso: $0 [-f|--force] [-v] <tamanho N> [<seed>]"
            echo "  -f, --force       Fuerza la compilación cruzada de todos los programas."
            echo "  -v                Muestra información adicional durante la ejecución."
            echo "  -h, --help        Muestra esta ayuda y sale."
            exit 0
            ;;      
        --)
            shift
            break
            ;;
        *)
            echo "Error interno en getopt"
            exit 1
            ;;
    esac
done

# Verificar si se proporcionaron al menos un parámetro posicional (tamanhoN)
if [ $# -lt 1 ]; then
    usage
fi

# Asignar argumentos posicionales
tamanhoN=$1
seed=${2:-}

# Comprobar que tamanhoN sea un número positivo mayor que 0
if ! [[ "$tamanhoN" =~ ^[0-9]+$ ]] || [ "$tamanhoN" -le 0 ]; then
    echo "Error: tamanho N debe ser un número positivo mayor que 0."
    exit 1
fi

# Si se proporciona seed, comprobar que sea un número positivo mayor que 0
if [ -n "$seed" ]; then
    if ! [[ "$seed" =~ ^[0-9]+$ ]] || [ "$seed" -le 0 ]; then
        echo "Error: seed debe ser un número positivo mayor que 0 si se proporciona."
        exit 1
    fi
fi

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
            script="${DIR}/${dir_lower}_run_${CPU_VENDOR}.sh"  # Formato: directorio/dir_run_ARCH.sh
            if [[ -f "$script" && -x "$script" ]]; then
                echo "Ejecutando $DIR"
                ./"$script" $tamanhoN $seed $force_flag $verbose_flag
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
            script="${DIR}/${dir_lower}_run_${CPU_VENDOR}.sh"  # Formato: directorio/dir_run_ARCH.sh
            if [[ -f "$script" && -x "$script" ]]; then
                echo "Ejecutando $DIR"
                ./"$script" $tamanhoN $seed $force_flag
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