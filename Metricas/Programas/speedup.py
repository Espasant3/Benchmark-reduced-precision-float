import csv
import sys
import argparse
from typing import List, Tuple

def leer_csv(ruta: str) -> List[Tuple[int, List[float]]]:
    """Lee un archivo CSV y devuelve una lista de tuplas (n, [tiempos])."""
    try:
        with open(ruta, "r", newline="", encoding="utf-8") as f:
            lector = csv.reader(f)
            encabezado = next(lector)  # Ignoramos el encabezado
            datos = []
            for num_fila, fila in enumerate(lector, start=2):  # Contamos desde la línea 2
                if not fila:
                    continue  # Saltar filas vacías
                try:
                    n = int(fila[0])
                    tiempos = [float(t) for t in fila[1:]]
                    datos.append((n, tiempos))
                except ValueError:
                    print(f"Error en {ruta}, fila {num_fila}: valores no numéricos.")
                    sys.exit(1)
            return datos
    except FileNotFoundError:
        print(f"Error: no se encontró el archivo '{ruta}'.")
        sys.exit(1)
    except PermissionError:
        print(f"Error: permiso denegado para leer '{ruta}'.")
        sys.exit(1)

def validar_datos(datos1: List[Tuple[int, List[float]]], datos2: List[Tuple[int, List[float]]]):
    """Verifica que ambos conjuntos de datos sean comparables."""
    if len(datos1) != len(datos2):
        print(f"Error: los archivos tienen diferente número de filas ({len(datos1)} vs {len(datos2)}).")
        sys.exit(1)

    for (n1, tiempos1), (n2, tiempos2) in zip(datos1, datos2):
        if n1 != n2:
            print(f"Error: valores de 'n' no coinciden: {n1} vs {n2}.")
            sys.exit(1)
        if len(tiempos1) != len(tiempos2):
            print(f"Error: número de tiempos no coincide para n={n1}.")
            sys.exit(1)

def calcular_speedups(datos1: List[Tuple[int, List[float]]], datos2: List[Tuple[int, List[float]]]) -> List[List[float]]:
    """Calcula el speedup entre dos conjuntos de datos."""
    speedups = []
    for (_, tiempos1), (_, tiempos2) in zip(datos1, datos2):
        fila_speedups = []
        for t1, t2 in zip(tiempos1, tiempos2):
            if t2 == 0:
                print(f"Error: división por cero en tiempo {t2}.")
                sys.exit(1)
            fila_speedups.append(t1 / t2)
        speedups.append(fila_speedups)
    return speedups

def escribir_csv_salida(ruta_salida: str, datos1: List[Tuple[int, List[float]]], speedups: List[List[float]]):
    """Escribe los resultados en un archivo CSV."""
    try:
        file_name = f"{ruta_salida}.csv"
        with open(file_name, "w", newline="", encoding="utf-8") as f:
            escritor = csv.writer(f)
            # Generar encabezado dinámico
            num_speedups = len(speedups[0])
            encabezado = ["n"] + [f"Speedup_{i+1}" for i in range(num_speedups)]
            escritor.writerow(encabezado)
            # Escribir filas
            for (n, _), sp in zip(datos1, speedups):
                escritor.writerow([n] + sp)
        print(f"Speedup calculado y guardado en '{file_name}'.")
    except PermissionError:
        print(f"Error: permiso denegado para escribir en '{ruta_salida}'.")
        sys.exit(1)

def parse_arguments():
    """Parsea los argumentos de línea de comandos con nombres claros."""
    parser = argparse.ArgumentParser(description="Calcula el speedup entre dos archivos CSV.")
    parser.add_argument("-s", "--source", required=True,
                        help="Ruta al archivo CSV con los tiempos base (original).")
    parser.add_argument("-n", "--new", required=True,
                        help="Ruta al archivo CSV con los tiempos mejorados (nuevo).")
    parser.add_argument("-o", "--output", default="speedup.csv",
                        help="Nombre del archivo de salida (por defecto: speedup.csv).")
    return parser.parse_args()

def main():
    args = parse_arguments()

    # Leer archivos
    datos_origen = leer_csv(args.source)
    datos_nuevos = leer_csv(args.new)

    # Validar compatibilidad
    validar_datos(datos_origen, datos_nuevos)

    # Calcular speedups
    speedups = calcular_speedups(datos_origen, datos_nuevos)

    # Guardar resultados
    escribir_csv_salida(args.output, datos_origen, speedups)

if __name__ == "__main__":
    main()