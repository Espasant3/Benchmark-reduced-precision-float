# Error Cuadrático Medio Normalizado (NRMSE) para comparar resultados de ejecuciones
import numpy as np
import csv
import argparse
from collections import defaultdict
from pathlib import Path

# Prefijos conocidos (fácilmente ampliable)
KNOWN_PREFIXES = ['axpy', 'dct', 'dwt_1d', 'pca', 'pca_reimpl']

def load_vector_data(filepath):
    data = defaultdict(list)
    with open(filepath, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            ejecucion = int(row['Ejecucion'])
            valor = float(row['Valor'])
            data[ejecucion].append(valor)
    
    ejecuciones = sorted(data.keys())
    return [np.array(data[e], dtype=float) for e in ejecuciones]

def load_matrix_data(filepath):
    data = defaultdict(list)
    with open(filepath, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            ejecucion = int(row['Ejecucion'])
            fila = int(row['Fila']) - 1
            columna = int(row['Columna']) - 1
            valor = float(row['Valor'])
            data[ejecucion].append((fila, columna, valor))
    
    max_fila = max([max([x[0] for x in data[e]]) for e in data]) + 1
    max_col = max([max([x[1] for x in data[e]]) for e in data]) + 1
    
    matrices = []
    for e in sorted(data.keys()):
        matrix = np.zeros((max_fila, max_col))
        for fila, columna, valor in data[e]:
            matrix[fila, columna] = valor
        matrices.append(matrix)
    
    return matrices

def normalized_rmse(y_true, y_pred):
    """Calcula el Error Cuadrático Medio Normalizado (NRMSE) usando el rango de los datos."""
    rmse = np.sqrt(np.mean((y_true - y_pred) ** 2))
    data_range = np.max(y_true) - np.min(y_true)
    if data_range == 0:
        return 0.0  # Evitar división por cero
    return rmse / data_range

def get_operation_name(filename):
    """Extrae el nombre de la operación del nombre del archivo"""
    stem = Path(filename).stem
    for prefix in KNOWN_PREFIXES:
        if stem.startswith(prefix):
            return prefix.upper()
    return "OPERACION"  # Valor por defecto si no coincide

def save_results_to_csv(output_name, operation, ref_name, results):
    """Guarda los resultados en un archivo CSV."""
    output_file = Path(__file__).parent / f"{output_name}.csv"

    # Determinar si hay solo una ejecución en todos los resultados
    single_execution = all(len(result['nrmse']) == 1 for result in results)

    with open(output_file, 'w') as f:
        # Escribir metadatos como comentarios
        f.write(f"# Resultados {operation}\n")
        f.write(f"# Referencia: {ref_name}\n")

        # Escribir encabezado según el número de ejecuciones
        if single_execution:
            f.write("Configuración,NRMSE\n")
        else:
            f.write("Configuración,Ejecución,NRMSE\n")

        # Escribir los resultados
        for result in results:
            for i, nrmse in enumerate(result['nrmse'], 1):
                if single_execution:
                    f.write(f"{result['name']},{nrmse:.8f}\n")
                else:
                    f.write(f"{result['name']},{i},{nrmse:.8f}\n")

    print(f"\nResultados guardados en {output_file}")
    print(f"Referencia utilizada: {ref_name}")

def parse_arguments():
    parser = argparse.ArgumentParser(description='Calcula el Error Cuadrático Medio Normalizado.')
    parser.add_argument('true_file', help='Archivo CSV de referencia (FP32)')
    parser.add_argument('pred_files', nargs='+', help='Archivos CSV a comparar')
    parser.add_argument('-t', '--type', choices=['vector', 'matrix'], required=True,
                      help='Tipo de datos: "vector" o "matrix"')
    parser.add_argument('-o', '--output', help='Nombre base para el archivo de resultados (sin extensión)')
    return parser.parse_args()

def main():
    args = parse_arguments()
    
    # Obtener nombre de la operación y referencia
    operation = get_operation_name(args.true_file)
    ref_name = Path(args.true_file).stem
    
    # Cargar datos de referencia
    if args.type == 'vector':
        true_data = load_vector_data(args.true_file)
    else:
        true_data = load_matrix_data(args.true_file)
    
    results = []
    
    for pred_file in args.pred_files:
        # Cargar datos de predicción
        try:
            if args.type == 'vector':
                pred_data = load_vector_data(pred_file)
            else:
                pred_data = load_matrix_data(pred_file)
        except FileNotFoundError:
            print(f"Error: Archivo {pred_file} no encontrado")
            continue
        
        # Validar número de ejecuciones
        if len(true_data) != len(pred_data):
            print(f"Error en {Path(pred_file).name}: Ejecuciones diferentes ({len(true_data)} vs {len(pred_data)})")
            continue
        
        # Calcular NRMSE para cada ejecución
        nrmse_values = []
        for y_true, y_pred in zip(true_data, pred_data):
            nrmse = normalized_rmse(y_true, y_pred)
            nrmse_values.append(nrmse)
        
        # Guardar resultados
        results.append({
            'name': Path(pred_file).stem,
            'nrmse': nrmse_values,
            'executions': len(nrmse_values)
        })
    
    # Mostrar resultados
    print(f"\nResultados {operation}:")
    print(f"Referencia: {ref_name}")
    for result in results:
        print(f"\n{result['name']} vs {ref_name}:")
        for i, nrmse in enumerate(result['nrmse'], 1):
            print(f"Ejecución {i}: {nrmse:.8f}")
    
    # Guardar resultados si se especifica un archivo de salida
    if args.output:
        save_results_to_csv(args.output, operation, ref_name, results)

if __name__ == '__main__':
    main()