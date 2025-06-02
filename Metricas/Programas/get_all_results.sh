#!/bin/bash

# --- Definición de funciones --- #
usage() {
    echo "Uso: $0 [opciones]"
    echo "Opciones:"
    echo "  -d, --directories    Lista de directorios a procesar (separados por espacios) relativos a script_dir/../../Programas/"
    echo "  -h, --help           Muestra esta ayuda y sale"
    exit 0
}

# --- Variables por defecto --- #
user_directories=()
DIRECTORIOS_DEFAULT=("AXPY" "DWT_1D" "PCA" "PCA_REIMPL" "DCT")

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
if [ "$ARCH" == "aarch64" ] || [ "$ARCH" == "arm64" ]; then
    ./../../Programas/compile_all.sh > /dev/null
else
    ./../../Programas/compile_all.sh -f > /dev/null
fi

echo "Compilación de los programas previa a la ejecucion terminada"

./../../Tests-Python/setup-python-env.sh

source ../../Tests-Python/entorno_TFG/bin/activate

# Determinar directorios a procesar
if [ ${#user_directories[@]} -gt 0 ]; then
    DIRECTORIOS=("${user_directories[@]}")
else
    DIRECTORIOS=("${DIRECTORIOS_DEFAULT[@]}")
fi

# FLAGS comunes para los programas
SEED_FLAGS="-s 1234"
case "$ARCH" in
    x86_64|amd64|x64)
        echo -e "\nStarting..."
        # Procesar cada directorio
        for DIR in "${DIRECTORIOS[@]}"; do
            echo "=========== Procesando $DIR ==========="

            if [ "$DIR" == "PCA" ] || [ "$DIR" == "PCA_REIMPL" ]; then
                # Modificar el número de ejecuciones para PCA y PCA_REIMPL
                FLAGS="-n 1000 $SEED_FLAGS -m -o"
            else
                FLAGS="-n 15000 $SEED_FLAGS -o"
            fi

            for file in ../../Programas/$DIR/*; do
                # Validar que sea un archivo ejecutable (no sh, out ni o)
                if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
                    echo "Procesando archivo: $file"
                    python3 resultado_ejecucion.py -p "$file" $FLAGS > /dev/null
                    echo "$file done"
                fi
            done
            echo "$DIR done"
        done

        echo "Procesando mediante emulación (QEMU) directorios para archivos .out"

        for DIR in "${DIRECTORIOS[@]}"; do
            echo "=========== Procesando $DIR (QEMU) ==========="

            if [ "$DIR" == "PCA" ] || [ "$DIR" == "PCA_REIMPL" ]; then
                # Modificar el número de ejecuciones para PCA y PCA_REIMPL
                FLAGS="-n 1000 $SEED_FLAGS -m -o -q"
            else
                FLAGS="-n 15000 $SEED_FLAGS -o -q"
            fi

            for file in ../../Programas/$DIR/*.out; do
                # Validar que sea un archivo ejecutable
                if [ -f "$file" ] && [ -x "$file" ]; then
                    echo "Procesando archivo (QEMU): $file"
                    python3 resultado_ejecucion.py -p "$file" $FLAGS > /dev/null
                    echo "$file done"
                fi
            done
            echo "$DIR (QEMU) done"
        done

        ;;

    aarch64|arm64)
        echo -e "\nStarting..."
        # Procesar cada directorio
        for DIR in "${DIRECTORIOS[@]}"; do
            echo "=========== Procesando $DIR ==========="       

            if [ "$DIR" == "PCA" ] || [ "$DIR" == "PCA_REIMPL" ]; then
                # Modificar el número de ejecuciones para PCA y PCA_REIMPL
                FLAGS="-n 1000 $SEED_FLAGS -m -o"
            else
                FLAGS="-n 15000 $SEED_FLAGS -o"
            fi

            for file in ../../Programas/$DIR/*.out; do
                # Validar que sea un archivo ejecutable
                if [ -f "$file" ] && [ -x "$file" ]; then
                    echo "Procesando archivo: $file"
                    python3 resultado_ejecucion.py -p "$file" $FLAGS > /dev/null
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