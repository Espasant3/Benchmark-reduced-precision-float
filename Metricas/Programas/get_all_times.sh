#/bin/bash

# Obtener el directorio donde está ubicado el script
script_dir="$(dirname "$0")"

# Cambiar al directorio del script
cd "$script_dir"

# Obtener la arquitectura del sistema
ARCH=$(uname -m)

./../../Programas/compile_all.sh

echo "Compilación de los programas previa a la ejecucion terminada"

./../../Tests-Python/setup-python-env.sh

source ../../Tests-Python/entorno_TFG/bin/activate

case "$ARCH" in
    x86_64|amd64|x64)

    echo "Starting..."

    for file in ../../Programas/AXPY/*; do
        if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
            python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
        fi
    done

    echo "AXPY done"

    for file in ../../Programas/DCT/*; do
        if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
            python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
        fi
    done

    echo "DCT done"

    for file in ../../Programas/DWT_1D/*; do
        if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
            python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
        fi
    done

    echo "DWT_1D done"

    for file in ../../Programas/PCA/*; do
        if [ -f "$file" ] && [ -x "$file" ] && [[ "$file" != *.sh ]] && [[ "$file" != *.out ]] && [[ "$file" != *.o ]]; then
            python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
        fi
    done

    echo "PCA done"

    ;;
    aarch64|arm64)

    echo "Starting..."

    for file in ../../Programas/AXPY/*.out; do
        python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 #> /dev/null
    done

    echo "AXPY done"

    for file in ../../Programas/DCT/*.out; do
        python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
    done

    echo "DCT done"

    for file in ../../Programas/DWT_1D/*.out; do
        python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
    done

    echo "DWT_1D done"

    for file in ../../Programas/PCA/*.out; do
        python3 tiempo_ejecucion_cache_limits_dynamic.py $file --show_plot false --save_plot false --save_data true --num_ejecuciones 15 > /dev/null
    done

    echo "PCA done"

    ;;
    *)
    echo "Arquitectura no soportada"
    ;;

esac

deactivate
