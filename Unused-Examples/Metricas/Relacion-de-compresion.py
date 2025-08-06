import numpy as np
import csv
import argparse
import os

def cargar_datos_csv(ruta_archivo):
    """Carga datos desde CSV y los devuelve como diccionario de ejecuciones."""
    datos = {}
    with open(ruta_archivo, 'r') as f:
        reader = csv.DictReader(f)
        for fila in reader:
            ejec = int(fila["Ejecucion"])
            if ejec not in datos:
                datos[ejec] = []
            datos[ejec].append(fila)
    return datos

def reconstruir_array(datos_ejecucion):
    """Convierte datos de 1D en un array de NumPy."""
    indices = [int(d["Indice"]) for d in datos_ejecucion]
    valores = [float(d["Valor"]) for d in datos_ejecucion]
    array = np.zeros(max(indices) + 1)  # +1 para índices base 1
    for i, val in zip(indices, valores):
        array[i - 1] = val  # Ajuste a base 0
    return array

def reconstruir_matriz(datos_ejecucion):
    """Convierte datos de 2D en una matriz de NumPy."""
    filas = max(int(d["Fila"]) for d in datos_ejecucion)
    cols = max(int(d["Columna"]) for d in datos_ejecucion)
    matriz = np.zeros((filas, cols))
    for d in datos_ejecucion:
        i, j, val = int(d["Fila"]), int(d["Columna"]), float(d["Valor"])
        matriz[i - 1, j - 1] = val  # Ajuste a base 0
    return matriz

def calcular_cr(original, comprimido):
    return original.size / comprimido.size

def parse_arguments():
    parser = argparse.ArgumentParser(description='Calcular relación de compresión')
    # Argumentos como flags opcionales (orden arbitrario)
    parser.add_argument(
        '-d', '--dimension',
        choices=['1d', '2d'],
        required=True,
        help='Dimensión de los datos (1d o 2d)'
    )
    parser.add_argument(
        '-o', '--original',
        required=True,
        help='Archivo original (CSV)'
    )
    parser.add_argument(
        '-c', '--comprimido',
        required=True,
        help='Archivo comprimido (CSV)'
    )
    parser.add_argument(
        '-e', '--ejecucion',
        type=int,
        help='Filtrar por ejecución específica'
    )
    return parser.parse_args()

def main():
    args = parse_arguments()

    # Verificar si los archivos existen
    if not os.path.isfile(args.original):
        print(f"Error: Archivo original '{args.original}' no encontrado.")
        return
    if not os.path.isfile(args.comprimido):
        print(f"Error: Archivo comprimido '{args.comprimido}' no encontrado.")
        return

    # Cargar datos
    datos_orig = cargar_datos_csv(args.original)
    datos_comp = cargar_datos_csv(args.comprimido)

    # Filtrar por ejecución (si se especifica)
    ejecuciones = [args.ejecucion] if args.ejecucion else datos_orig.keys()

    for ejec in ejecuciones:
        print(f"\n--- Ejecución {ejec} ---")
        orig_ejec = datos_orig[ejec]
        comp_ejec = datos_comp.get(ejec, [])

        if args.dimension == '1d':
            señal_orig = reconstruir_array(orig_ejec)
            señal_comp = reconstruir_array(comp_ejec)
        elif args.dimension == '2d':
            señal_orig = reconstruir_matriz(orig_ejec)
            señal_comp = reconstruir_matriz(comp_ejec)

        cr = calcular_cr(señal_orig, señal_comp)
        print(f"Relación de compresión (CR): {cr:.5f}")

if __name__ == "__main__":
    main()