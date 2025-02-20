#!/bin/bash

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

# Directorios a procesar (se puede expandir fácilmente)
DIRECTORIOS=("AXPY" "DWT_1D" "PCA" "DCT")

EJECUCIONES=13

# FLAGS comunes para los programas
FLAGS="--show_plot false --save_plot false --save_data true"

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
                NUM_EJECUCIONES=$EJECUCIONES
            fi
            
            FLAGS_WITH_NUM="--num_ejecuciones $NUM_EJECUCIONES $FLAGS"

            for file in ../../Programas/$DIR/*; do
                # Validar que sea un archivo ejecutable (no sh, out ni o)
                if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
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
            for file in ../../Programas/$DIR/*.out; do

            # Modificar el número de ejecuciones si es el directorio DCT
            if [ "$DIR" == "DCT" ]; then
                NUM_EJECUCIONES=7
            else
                NUM_EJECUCIONES=$EJECUCIONES
            fi
            
            FLAGS_WITH_NUM="--num_ejecuciones $NUM_EJECUCIONES $FLAGS"
                # Validar que sea un archivo ejecutable
                if [ -f "$file" ] && [ -x "$file" ]; then
                    python3 tiempo_ejecucion_cache_limits_dynamic.py "$file" $FLAGS > /dev/null
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