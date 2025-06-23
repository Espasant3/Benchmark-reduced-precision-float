# Coeficiente de Correlación de Pearson entre un archivo original y otro procesado
import numpy as np
import csv
import argparse
from collections import defaultdict
from pathlib import Path

# Prefijos conocidos (fácilmente ampliables)
KNOWN_PREFIXES = ['axpy', 'dct', 'dwt_1d', 'pca', 'pca_reimpl']

def load_vector_data(filepath):
    """Carga datos vectoriales desde CSV agrupados por ejecución."""
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
    """Carga datos matriciales desde CSV agrupados por ejecución."""
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

def pearson_correlation(y_true, y_pred):
    """Calcula el Coeficiente de Correlación de Pearson."""
    y_true = np.asarray(y_true)
    y_pred = np.asarray(y_pred)
    
    if y_true.shape != y_pred.shape:
        raise ValueError("Las formas de y_true y y_pred no coinciden")
    
    mean_true = np.mean(y_true)
    mean_pred = np.mean(y_pred)
    
    numerator = np.sum((y_true - mean_true) * (y_pred - mean_pred))
    denominator = np.sqrt(np.sum((y_true - mean_true)**2) * np.sum((y_pred - mean_pred)**2))
    
    if denominator == 0:
        return 0.0
    
    return numerator / denominator

def get_operation_name(filename):
    """Extrae el nombre de la operación del nombre del archivo"""
    stem = Path(filename).stem
    for prefix in KNOWN_PREFIXES:
        if stem.startswith(prefix):
            return prefix.upper()
    return "OPERACION"

def save_results_to_csv(output_name, operation, source_name, postop_name, result):
    """Guarda los resultados en un archivo CSV."""
    output_file = Path(__file__).parent / f"{output_name}.csv"

    # Determinar si hay solo una ejecución
    single_execution = len(result['pearson']) == 1

    with open(output_file, 'w') as f:
        # Escribir metadatos como comentarios
        f.write(f"# Resultados {operation}\n")
        f.write(f"# Referencia: {source_name}\n")
        f.write(f"# Procesado: {postop_name}\n")

        # Escribir encabezado según el número de ejecuciones
        if single_execution:
            f.write("Comparación,Pearson\n")
        else:
            f.write("Comparación,Ejecución,Pearson\n")

        # Escribir los resultados
        for i, pearson in enumerate(result['pearson'], 1):
            if single_execution:
                f.write(f"{postop_name},{pearson:.8f}\n")
            else:
                f.write(f"{postop_name},{i},{pearson:.8f}\n")

    print(f"\nResultados guardados en {output_file}")
    print(f"Referencia utilizada: {source_name}")
    print(f"Archivo procesado: {postop_name}")

def parse_arguments():
    parser = argparse.ArgumentParser(description='Calcula el Coeficiente de Correlación de Pearson entre un archivo original y otro procesado.')
    parser.add_argument('-s', '--source', required=True, help='Archivo CSV de referencia (FP32)')
    parser.add_argument('-p', '--post-operations', required=True, help='Archivo CSV procesado')
    parser.add_argument('-t', '--type', choices=['vector', 'matrix'], required=True,
                      help='Tipo de datos: "vector" o "matrix"')
    parser.add_argument('-o', '--output', help='Nombre base para el archivo de resultados (sin extensión)')
    return parser.parse_args()

def main():
    args = parse_arguments()
    
    # Obtener nombre de la operación y referencias
    operation = get_operation_name(args.post_operations)
    source_name = Path(args.source).stem
    postop_name = Path(args.post_operations).stem
    
    # Cargar datos de referencia
    if args.type == 'vector':
        source_data = load_vector_data(args.source)
    else:
        source_data = load_matrix_data(args.source)
    
    # Cargar datos procesados
    try:
        if args.type == 'vector':
            postop_data = load_vector_data(args.post_operations)
        else:
            postop_data = load_matrix_data(args.post_operations)
    except FileNotFoundError:
        print(f"Error: Archivo {args.post_operations} no encontrado")
        return
    
    # Validar número de ejecuciones
    if len(source_data) != len(postop_data):
        print(f"Error: Ejecuciones diferentes ({len(source_data)} vs {len(postop_data)})")
        return
    
    # Calcular Pearson para cada ejecución
    pearson_values = []
    for y_true, y_pred in zip(source_data, postop_data):
        pearson = pearson_correlation(y_true, y_pred)
        pearson_values.append(pearson)
    
    result = {
        'pearson': pearson_values
    }
    
    # Mostrar resultados
    print(f"\nResultados {operation}:")
    print(f"Referencia: {source_name}")
    print(f"Procesado: {postop_name}")
    for i, pearson in enumerate(result['pearson'], 1):
        print(f"Ejecución {i}: {pearson:.8f}")
    
    # Guardar resultados si se especifica un archivo de salida
    if args.output:
        save_results_to_csv(args.output, operation, source_name, postop_name, result)

if __name__ == '__main__':
    main()