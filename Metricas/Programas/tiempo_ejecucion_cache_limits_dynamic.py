import subprocess
import argparse
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import csv
import statistics
import os
import math
import numpy as np
import re

def get_cache_info():
    """
    Detecta los tamaños de caché y RAM de la CPU.
    Devuelve una tupla con (L1, L2, L3, RAM_LIMITS).
    Si no se puede detectar, usa valores por defecto.
    """
    try:
        result = subprocess.run(['lscpu'], stdout=subprocess.PIPE, text=True, check=True)
        output = result.stdout

        # Diccionario para almacenar los tamaños de caché
        cache_sizes = {}

        # Patrones de búsqueda para caché (adaptados al nuevo formato)
        cache_patterns = {
            "L1d": r"L1d:\s+([\d.]+ [KMG]iB) \((\d+) instance[s]\)",
            "L2": r"L2:\s+([\d.]+ [KMG]iB) \((\d+) instance[s]\)",
            "L3": r"L3:\s+([\d.]+ [KMG]iB) \((\d+) instance[s]?\)"  # Maneja singular/plural
        }

        # Buscar los tamaños de caché en la salida
        for cache_type, pattern in cache_patterns.items():
            match = re.search(pattern, output)
            if match:
                cache_size_str, instance_count = match.groups()
                instance_count = int(instance_count)

                # Convertir el tamaño a bytes para facilitar cálculos
                if "KiB" in cache_size_str:
                    cache_size = float(cache_size_str.replace("KiB", "")) * 1024
                elif "MiB" in cache_size_str:
                    cache_size = float(cache_size_str.replace("MiB", "")) * 1024 * 1024
                elif "GiB" in cache_size_str:
                    cache_size = float(cache_size_str.replace("GiB", "")) * 1024 * 1024 * 1024
                else:
                    cache_size = float(cache_size_str)  # Asumimos bytes si no hay unidad

                # Dividir el tamaño total entre el número de instancias para obtener el tamaño por instancia
                cache_sizes[cache_type] = cache_size / instance_count
            else:
                cache_sizes[cache_type] = 0  # Si no se encuentra, asumimos 0

        # Calcular RAM proporcional (64MB = 5x L3, 512MB = 40x L3)
        if 'L3' in cache_sizes:
            ram_limits = [
                5 * cache_sizes['L3'],    # Equivalente a 64 MiB para L3=12.8MB
                40 * cache_sizes['L3']    # Equivalente a 512 MiB
            ]
        else:
            ram_limits = [64*1024**2, 512*1024**2]  # Valores por defecto
        # Devolver los valores directamente
        return (
            cache_sizes.get('L1d', 48*1024),    # L1 en bytes
            cache_sizes.get('L2', 1.25*1024**2),  # L2 en bytes
            cache_sizes.get('L3', 12*1024**2),    # L3 en bytes
            ram_limits                           # RAM en bytes
        )

    except (subprocess.CalledProcessError, FileNotFoundError):
        print("No se pudo detectar lscpu, usando valores por defecto")
        return (
            48*1024,            # L1 por defecto (48 KiB)
            1.25*1024**2,       # L2 por defecto (1.25 MiB)
            12*1024**2,         # L3 por defecto (12 MiB)
            [64*1024**2, 512*1024**2]  # RAM por defecto (64 MiB, 512 MiB)
        )

# Obtener límites de memoria dinámicamente
L1_CACHE_LIMIT, L2_CACHE_LIMIT, L3_CACHE_LIMIT, RAM_LIMITS = get_cache_info()

def calcular_n_para_memoria(memoria_bytes, tipo_dato, nombre_programa):
    """
    Calcula el valor de n necesario para alcanzar una cantidad de memoria dada,
    considerando el tipo de dato y el nombre del programa.

    Parámetros:
        memoria_bytes (int): Cantidad de memoria en bytes.
        tipo_dato (str): Tipo de dato ("half" u otro).
        nombre_programa (str): Nombre del programa específico.

    Retorna:
        int: Valor de n calculado.
    """
    # Determinar el tamaño en bytes de cada elemento según el tipo de dato
    bytes_por_elemento = 2 if tipo_dato == "half" else 4

    # Caso especial para matrices cuadradas (PCA)
    if 'pca' in nombre_programa.lower():
        return int(math.sqrt(memoria_bytes / bytes_por_elemento))

    # Manejo de casos específicos según el programa
    elif "dwt_1d" in nombre_programa.lower():
        # Dos arrays de tamaño n (tipo principal) + kernels (16 doubles)
        memoria_kernels = (7 + 9) * 8  # 16 elementos de 8 bytes cada uno
        memoria_disponible = memoria_bytes - memoria_kernels
        if memoria_disponible <= 0:
            raise ValueError("La memoria disponible no es suficiente para los kernels.")
        return int(memoria_disponible / (2 * bytes_por_elemento))

    else:
        # Por defecto: dos arrays de tamaño n (incluye AXPY, DCT y otros similares)
        return int(memoria_bytes / (2 * bytes_por_elemento))

def detectar_tipo_dato(nombre_programa):
    """Detecta el tipo de dato basándose en sufijos en el nombre."""
    if any(sufijo in nombre_programa for sufijo in ('_FP16', '_FP16_ARM', '_BF16')):
        return "half"
    return "float"  # Por defecto o si contiene _FP32

def detectar_num_tiempos(programa, n_minimo, seed):
    try:
        proceso = subprocess.run(
            [programa, str(n_minimo), str(seed)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=10
        )
        return len([linea for linea in proceso.stdout.split('\n') if 'Tiempo de ejecucion:' in linea])
    except Exception as e:
        print(f"Error detectando tiempos: {e}")
        return 1

def medir_tiempos(programa, valores_n, seed, num_ejecuciones=11):
    n_minimo = min(valores_n)
    num_tiempos = detectar_num_tiempos(programa, n_minimo, seed)
    print(f"Tiempos detectados por ejecución: {num_tiempos}")
    
    tiempos_por_n = []

    for n in valores_n:
        tiempos = [[] for _ in range(num_tiempos)]  # Lista para almacenar tiempos por ejecución
        
        for _ in range(num_ejecuciones):
            try:
                # Ejecutar el programa con subprocess.run()
                resultado = subprocess.run(
                    [programa, str(n), str(seed)],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                )
                
                # Procesar la salida estándar del programa
                tiempos_ejecucion = []
                for linea in resultado.stdout.split('\n'):
                    if 'Tiempo de ejecucion:' in linea:
                        tiempo = float(linea.split(':')[1].strip())
                        tiempos_ejecucion.append(tiempo)
                
                # Validar que se hayan obtenido los tiempos esperados
                if len(tiempos_ejecucion) == num_tiempos:
                    for i in range(num_tiempos):
                        tiempos[i].append(tiempos_ejecucion[i])
                else:
                    print(f"Advertencia: n={n} tiene {len(tiempos_ejecucion)} tiempos vs {num_tiempos} esperados")
            
            except Exception as e:
                print(f"Error en n={n}: {e}")
                continue
        
        # Calcular las medianas de los tiempos recolectados
        medianas = [statistics.median(t) if t else None for t in tiempos]
        tiempos_por_n.append(medianas)
        print(f"n={n}: {medianas}")
    
    return tiempos_por_n, num_tiempos

def guardar_datos_csv(nombre_archivo, valores_n, tiempos_por_n, num_tiempos):
    """Guarda los datos en un archivo CSV."""
    with open(nombre_archivo, 'w') as f:
        writer = csv.writer(f)
        writer.writerow(['n'] + [f'Tiempo_{i+1}' for i in range(num_tiempos)])
        for n, t in zip(valores_n, tiempos_por_n):
            writer.writerow([n] + t)
    print(f"Datos guardados en {nombre_archivo}")

def guardar_datos_grafica(nombre_archivo):
    """Guarda la gráfica en un archivo PNG."""
    plt.savefig(nombre_archivo, bbox_inches='tight')
    print(f"Gráfica guardada en {nombre_archivo}")

def guardar_datos(nombre_programa, valores_n, tiempos_por_n, num_tiempos):
    """Guarda los datos en CSV y la gráfica en PNG."""
    n_inicial = min(valores_n)
    n_final = max(valores_n)
    num_puntos = len(valores_n)
    nombre_archivo_base = f"{nombre_programa}_ni{n_inicial}_nf{n_final}_ptos{num_puntos}_limits"
    
    # Guardar CSV
    csv_path = f"datos_{nombre_archivo_base}.csv"
    guardar_datos_csv(csv_path, valores_n, tiempos_por_n, num_tiempos)
    
    # Guardar gráfica
    png_path = f"grafico_{nombre_archivo_base}.png"
    guardar_datos_grafica(png_path)

def plot_grafica(valores_n, tiempos_por_n, num_tiempos, 
                 tipo_dato, save_data=False, nombre_programa=None):
    """
    Muestra la gráfica de tiempos de ejecución con límites ajustados.
    Usa el nombre del programa para determinar el tipo de prueba.
    """
    fig, ax = plt.subplots(figsize=(12, 7))
    
    # Configuración de ejes
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.grid(True, which='both', linestyle='--')
    ax.set_xlabel("Tamaño de n (elementos)", fontsize=12)
    ax.set_ylabel("Tiempo de ejecución (s)", fontsize=12)

    # Personalización de la cuadrícula
    ax.grid(True, which='major', linestyle='--', linewidth=0.5, color='gray', alpha=0.7)
    ax.grid(False, which='minor')  # Desactivar líneas secundarias

    # Límites de memoria
    limites = {
        "L1": L1_CACHE_LIMIT,
        "L2": L2_CACHE_LIMIT,
        "L3": L3_CACHE_LIMIT,
        "RAM 64MiB": RAM_LIMITS[0],
        "RAM 512MiB": RAM_LIMITS[1]
    }

    # Verificar si los límites son alcanzables
    n_max = max(valores_n)
    for nombre, limite in reversed(limites.items()):
        n_limite = calcular_n_para_memoria(limite, tipo_dato, nombre_programa)
        # Solo dibujar si el límite es alcanzable
        if n_max >= n_limite:
            ax.axvline(n_limite, color='orange', linestyle='--', alpha=0.7)
            ax.text(n_limite, ax.get_ylim()[0]*1.2, f'{nombre}\n(n={n_limite})', 
                    rotation=90, ha='center', va='bottom', fontsize=9)
        else:
            print(f"Límite no alcanzable: {nombre} (n={n_limite})")

    # Datos
    for i in range(num_tiempos):
        x = [n for n, t in zip(valores_n, tiempos_por_n) if t[i] is not None]
        y = [t[i] for t in tiempos_por_n if t[i] is not None]
        ax.plot(x, y, marker='o', linestyle='-', label=f'Tiempo {i+1}')

    ax.legend()

    # Guardar datos si es necesario
    if save_data and nombre_programa:
        guardar_datos(nombre_programa, valores_n, tiempos_por_n, num_tiempos)

    plt.show()

def main():
    parser = argparse.ArgumentParser(description="Benchmark de algoritmos")
    parser.add_argument("programa", help="Ruta al ejecutable a probar")
    parser.add_argument("--seed", type=int, default=1234)
    #parser.add_argument("--timeout", type=int, default=300)
    parser.add_argument("--save_data", type=lambda x: x.lower() == 'true', default=False, 
                        help="Guardar datos en CSV y gráfica (predeterminado: False)")
    args = parser.parse_args()

    # Extraer el nombre base del programa (sin ruta ni extensión)
    nombre_programa = args.programa.split('/')[-1].split('.')[0]

    # Detección automática
    tipo_dato = detectar_tipo_dato(nombre_programa)
    
    # Determinar el tipo de prueba basándose en el nombre del programa
    tipo_prueba = "matrix" if "pca" in nombre_programa.lower() else "array"

    # Valores predefinidos
    n_float = [
        4096, 6144, 8192, 12288, 16384, 32768, 65536, 98304, 
        131072, 163840, 196608, 262144, 327680, 524288, 1048576, 
        1572864, 2097152, 3145728, 4194304, 8388608, 12582912, 
        16777216, 20971520, 33554432, 67108864, 134217728
    ]
    n_16bit = [2 * x for x in n_float]
    n_float_matrices = [
        2, 10, 50, 100, 200,  # Rango inicial (crecimiento no crítico)
        300, 400, 500, 600, 700, 800, 900, 1000,  # Transición a crecimiento cúbico
        1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200  # Zona de interés
    ]   
    n_16bit_matrices = [int(1.4142 * x) for x in n_float_matrices]

    # Límites de memoria
    limites = {
        "L1": L1_CACHE_LIMIT,
        "L2": L2_CACHE_LIMIT,
        "L3": L3_CACHE_LIMIT,
        "RAM 64MiB": RAM_LIMITS[0],
        "RAM 512MiB": RAM_LIMITS[1]
    }

    # Selección de valores
    if tipo_prueba == 'array':
        valores_n = n_16bit if tipo_dato == 'half' else n_float
    else:
        valores_n = n_16bit_matrices if tipo_dato == 'half' else n_float_matrices

    print(f"Configuración detectada: {tipo_prueba.upper()} - {tipo_dato.upper()}")
    print(f"Valores de n: {valores_n}")

    # Medición
    tiempos, num_tiempos = medir_tiempos(args.programa, valores_n, args.seed)

    # Mostrar gráfica y guardar datos si es necesario
    plot_grafica(valores_n, tiempos, num_tiempos, tipo_dato, args.save_data, nombre_programa)

if __name__ == "__main__":
    main()
