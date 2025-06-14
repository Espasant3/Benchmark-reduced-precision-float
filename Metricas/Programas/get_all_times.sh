#!/bin/bash

# --- Definición de funciones --- #
usage() {
    echo "Uso: $0 [opciones]"
    echo "Opciones:"
    echo "  -d, --directories    Lista de directorios a procesar (separados por espacios) relativos a script_dir/../../Programas/"
    echo "  -e, --extended       Emplea la lista extendida de valores de n"
    echo "  -h, --help           Muestra esta ayuda y sale"
    exit 0
}

# --- Variables por defecto --- #
user_directories=()
DIRECTORIOS_DEFAULT=("AXPY" "DWT_1D" "PCA" "PCA_REIMPL" "DCT")

# FLAGS comunes para los programas
FLAGS="--show_plot false --save_plot false --save_data true"

# --- Procesar argumentos --- #
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            ;;
        -d|--directories)
            shift
            # Recoger todos los directorios hasta el siguiente flag
            while [[ $# -gt 0 && ! "$1" =~ ^- ]]; do
                user_directories+=("$1")
                shift
            done
            ;;
        -e|--extended)
            # Emplear la lista extendida de valores de n
            FLAGS="$FLAGS --n_extended"
            echo "Empleando lista extendida de valores de n"
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Argumento no reconocido: $1"
            exit 1
            ;;
    esac
done

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

# Obtener la arquitectura del sistema
ARCH=$(uname -m)

# Compilar todos los programas
./../../Programas/compile_all.sh

echo "Compilación de los programas previa a la ejecucion terminada"

./../../Tests-Python/setup-python-env.sh

source ../../Tests-Python/entorno_TFG/bin/activate

# Determinar directorios a procesar
if [ ${#user_directories[@]} -gt 0 ]; then
    DIRECTORIOS=("${user_directories[@]}")
else
    DIRECTORIOS=("${DIRECTORIOS_DEFAULT[@]}")
fi

case "$ARCH" in
    x86_64|amd64|x64)
        echo -e "\nStarting..."
        # Procesar cada directorio
        for DIR in "${DIRECTORIOS[@]}"; do
            echo "=========== Procesando $DIR ==========="

            # Modificar el número de ejecuciones si es el directorio DCT
            if [ "$DIR" == "DCT" ]; then
                NUM_EJECUCIONES=7
            else
                NUM_EJECUCIONES=13
            fi
            
            FLAGS_WITH_NUM="--num_ejecuciones $NUM_EJECUCIONES $FLAGS"

            for file in ../../Programas/$DIR/*; do
                # Validar que sea un archivo ejecutable (no sh, out ni o)
                if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
                    echo "Procesando archivo: $file"
                    python3 tiempo_ejecucion_cache_limits_dynamic.py "$file" $FLAGS_WITH_NUM > /dev/null
                    echo "$file done"
                fi
            done
            echo "$DIR done"
        done
        ;;

    aarch64|arm64)
        echo -e "\nStarting..."
        # Procesar cada directorio
        for DIR in "${DIRECTORIOS[@]}"; do
            echo "=========== Procesando $DIR ==========="       

            # Modificar el número de ejecuciones si es el directorio DCT
            if [ "$DIR" == "DCT" ]; then
                NUM_EJECUCIONES=7
            else
                NUM_EJECUCIONES=13
            fi
            
            FLAGS_WITH_NUM="--num_ejecuciones $NUM_EJECUCIONES $FLAGS"

            for file in ../../Programas/$DIR/*.out; do
                # Validar que sea un archivo ejecutable
                if [ -f "$file" ] && [ -x "$file" ]; then
                    echo "Procesando archivo: $file"
                    python3 tiempo_ejecucion_cache_limits_dynamic.py "$file" $FLAGS_WITH_NUM > /dev/null
                    echo "$file done"
                fi
            done
            echo "$DIR done"
        done
        ;;

    *)
        echo "Arquitectura no soportada"
        ;;
esac

deactivate