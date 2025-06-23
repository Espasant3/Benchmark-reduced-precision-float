import csv
import argparse
import sys


def read_csv_file(file_path):
    """Lee un archivo CSV y devuelve las cabeceras y los datos."""
    try:
        with open(file_path, 'r') as f:
            reader = csv.reader(f)
            headers = next(reader)
            data = []
            for row in reader:
                if len(row) != len(headers):
                    raise ValueError(f"Mismatch en el número de columnas en {file_path}")
                data.append(row)
            return headers, data
    except Exception as e:
        raise RuntimeError(f"Error leyendo {file_path}: {e}")

def validate_headers(all_headers):
    """Valida que todas las cabeceras sean iguales."""
    first_header = all_headers[0]
    for i, header in enumerate(all_headers):
        if header != first_header:
            raise ValueError(f"Las cabeceras del archivo {i+1} no coinciden con las del primer archivo.")

def process_data(all_data):
    """Procesa los datos agrupándolos por 'n'."""
    processed = {}
    
    for headers, data_rows in all_data:
        for row in data_rows:
            try:
                n_value = int(row[0])
            except ValueError:
                raise ValueError(f"Valor no numérico en la columna 'n' para la fila {row}")
                
            try:
                times = [float(value) for value in row[1:]]
            except ValueError:
                raise ValueError(f"Valor no numérico en los tiempos para la fila {row}")
                
            if n_value not in processed:
                processed[n_value] = []
            processed[n_value].append(times)
    
    return processed

def calculate_median(numbers):
    """Calcula la mediana de una lista de números."""
    if not numbers:
        return None
        
    sorted_numbers = sorted(numbers)
    mid = len(sorted_numbers) // 2
    
    if len(sorted_numbers) % 2 == 0:
        return (sorted_numbers[mid-1] + sorted_numbers[mid]) / 2
    else:
        return sorted_numbers[mid]

def write_output_file(file_path, data, headers):
    """Escribe los resultados en el archivo de salida."""
    try:
        file_name = f"{file_path}.csv"
        with open(file_name, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(headers)  # Escribir cabeceras originales
            
            for n_value in sorted(data.keys()):
                medians = []
                for col_idx in range(len(headers)-1):  # Para cada columna de tiempo
                    column_values = [times[col_idx] for times in data[n_value]]
                    medians.append(calculate_median(column_values))
                
                writer.writerow([n_value] + medians)
    except Exception as e:
        raise RuntimeError(f"Error escribiendo el archivo de salida: {e}")

def parse_arguments():
    """Parsea los argumentos de línea de comandos."""
    parser = argparse.ArgumentParser(description='Calcula la mediana de múltiples archivos CSV.')
    parser.add_argument('input_files', nargs='+', help='Archivos CSV de entrada')
    parser.add_argument('-o', '--output', required=True, help='Archivo CSV de salida')
    return parser.parse_args()

def main():
    """Función principal."""
    args = parse_arguments()
    
    if len(args.input_files) < 1:
        print("Se requiere al menos un archivo de entrada.")
        sys.exit(1)
    
    try:
        # Leer todos los archivos
        all_files_data = []
        all_headers = []
        
        for file in args.input_files:
            headers, data = read_csv_file(file)
            all_headers.append(headers)
            all_files_data.append((headers, data))
        
        # Validar cabeceras
        validate_headers(all_headers)
        
        # Procesar datos
        processed_data = process_data(all_files_data)
        
        # Escribir salida
        write_output_file(args.output, processed_data, all_headers[0])
        
        print(f"Mediana calculada correctamente y guardada en {args.output}.csv")
    
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()