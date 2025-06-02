import os
import re
import csv
import argparse
from pathlib import Path
import subprocess
import platform


def get_cpu_vendor():
    """Detecta el fabricante del CPU específicamente para sistemas Linux"""
    try:
        with open('/proc/cpuinfo', 'r') as f:
            for line in f:
                if line.startswith('vendor_id'):
                    vendor_str = line.split(':', 1)[1].strip()  # Normalizado a minúsculas
                    
                    # Detección mejorada usando múltiples patrones
                    if any(keyword in vendor_str.lower() for keyword in ['intel', 'genuineintel']):
                        return "Intel"

                    if any(keyword in vendor_str.lower() for keyword in ['amd', 'authenticamd']):
                        return "AMD"
                    
                    # Si no coincide con Intel/AMD, devolver el valor original
                    return vendor_str  # Devuelve el valor original si no es Intel ni AMD

    except FileNotFoundError:
        return "Unknown (File not found)"
    
    return "Unknown"

def medir_resultados(programa, n, seed, use_qemu, matrix_mode, is_origin_data):
    try:
        # Determinar el comando a ejecutar
        if use_qemu:
            comando = ["qemu-aarch64", programa, str(n), str(seed), "-v"]
        else:
            comando = [programa, str(n), str(seed), "-v"]
        
        # Ejecutar el programa
        resultado = subprocess.run(
            comando,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        
        # Capturar resultados (array o matriz)
        origin_data = []
        exec_results = []
        
        lines = resultado.stdout.split('\n')
        i = 0

        while i < len(lines):
            if is_origin_data and "Datos ejecucion:" in lines[i]:
                # Capturar datos de ORIGEN
                current_data = []
                
                if matrix_mode:
                    # Matriz: capturar n líneas con tabulación
                    i += 1  # Saltar a la primera línea de datos
                    for _ in range(n):
                        if i < len(lines) and lines[i].startswith('\t'):
                            fila = lines[i].strip().split()
                            if len(fila) != n:
                                print(f"Error: Fila con {len(fila)} elementos (esperados: {n})")
                                return None
                            current_data.extend([float(v) for v in fila])
                            i += 1
                    # Validar total de elementos
                    if len(current_data) != n * n:
                        print(f"Error en matriz origen: {len(current_data)} elementos (esperados: {n*n})")
                        return None
                else:
                    # Array: los datos están en la misma línea
                    # Extraer los valores después de ":"
                    valores = lines[i].split(":")[1].strip().split()
                    if len(valores) != n:
                        print(f"Error en array origen: {len(valores)} elementos (esperados: {n})")
                        return None
                    current_data = [float(v) for v in valores]
                    i += 1
                
                origin_data.append(current_data)
                
            elif "Resultados ejecucion:" in lines[i]:
                # Capturar RESULTADOS
                current_results = []
                
                if matrix_mode:
                    # Matriz: capturar n líneas con tabulación
                    i += 1  # Saltar a la primera línea de datos
                    for _ in range(n):
                        if i < len(lines) and lines[i].startswith('\t'):
                            fila = lines[i].strip().split()
                            if len(fila) != n:
                                print(f"Error: Fila con {len(fila)} elementos (esperados: {n})")
                                return None
                            current_results.extend([float(v) for v in fila])
                            i += 1
                    # Validar total de elementos
                    if len(current_results) != n * n:
                        print(f"Error en matriz resultados: {len(current_results)} elementos (esperados: {n*n})")
                        return None
                else:
                    # Array: los datos están en la misma línea
                    # Extraer los valores después de ":"
                    valores = lines[i].split(":")[1].strip().split()
                    if len(valores) != n:
                        print(f"Error en array resultados: {len(valores)} elementos (esperados: {n})")
                        return None
                    current_results = [float(v) for v in valores]
                    i += 1
                
                exec_results.append(current_results)
                
            else:
                i += 1

        return {
            'origin_data': origin_data,
            'exec_results': exec_results
        }
            
    except Exception as e:
        print(f"Error: {e}")
        return None

def move_generated_files(output_files, is_origin_data):
    script_dir = Path(__file__).resolve().parent
    architecture = platform.machine()
    moved_files = []

    # Determinar directorios base según arquitectura
    if architecture in ["x86_64", "amd64", "AMD64"]:
        vendor = get_cpu_vendor()
        base_dir = script_dir / ".." / "Datos" / "Resultados-ejecucion" 
        target_dir = base_dir / vendor if vendor not in ["Unknown", "Unknown (File not found)"] else base_dir / "x86_64"
    elif architecture in ["aarch64", "arm64"]:
        base_dir = script_dir / ".." / "Datos" / "Resultados-ejecucion"
        target_dir = base_dir / "ARM"
    else:
        print(f"Arquitectura no soportada: {architecture}")
        return output_files  # No mover archivos

    # Mover cada archivo generado
    for file_path in output_files:
        file_name = file_path.name
        
        # Determinar tipo de archivo por su prefijo
        if file_name.startswith("source_"):
            dest_dir = target_dir / "Datos-previos-operaciones"
        else:
            dest_dir = target_dir

        # Crear directorio si no existe
        dest_dir.mkdir(parents=True, exist_ok=True)
        
        # Mover archivo
        dest_path = dest_dir / file_name
        file_path.rename(dest_path)
        moved_files.append(dest_path)
        print(f"Archivo movido: {file_name} -> {dest_dir.resolve()}")

    return moved_files

def save_to_csv(results, programa, n, seed, matrix_mode, is_origin_data, use_qemu):
    """Guarda los resultados de la ejecución en archivos CSV."""
    script_dir = Path(__file__).resolve().parent
    programa_nombre = Path(programa).stem
    generated_files = []

    # Añadir sufijo QEMU si se usa emulación
    qemu_suffix = "_QEMU" if use_qemu else ""

    # 1. Guardar datos de ORIGEN solo si se usa el flag -o y hay datos disponibles
    if is_origin_data and results['origin_data']:
        origin_file = script_dir / f"source_{programa_nombre}_n{n}_seed{seed}{qemu_suffix}.csv"
        with open(origin_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            if matrix_mode:
                writer.writerow(["Ejecucion", "Fila", "Columna", "Valor"])
                for ejecucion, data in enumerate(results['origin_data'], 1):
                    idx = 0
                    for fila in range(n):
                        for col in range(n):
                            writer.writerow([ejecucion, fila+1, col+1, data[idx]])
                            idx += 1
            else:
                writer.writerow(["Ejecucion", "Indice", "Valor"])
                for ejecucion, data in enumerate(results['origin_data'], 1):
                    for idx, valor in enumerate(data, 1):
                        writer.writerow([ejecucion, idx, valor])
        generated_files.append(origin_file)
        print(f"Datos ORIGEN guardados: {origin_file.name}")

    # 2. Guardar RESULTADOS siempre que existan (comportamiento base)
    if results['exec_results']:
        results_file = script_dir / f"{programa_nombre}_n{n}_seed{seed}{qemu_suffix}.csv"
        with open(results_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            if matrix_mode:
                writer.writerow(["Ejecucion", "Fila", "Columna", "Valor"])
                for ejecucion, data in enumerate(results['exec_results'], 1):
                    idx = 0
                    for fila in range(n):
                        for col in range(n):
                            writer.writerow([ejecucion, fila+1, col+1, data[idx]])
                            idx += 1
            else:
                writer.writerow(["Ejecucion", "Indice", "Valor"])
                for ejecucion, data in enumerate(results['exec_results'], 1):
                    for idx, valor in enumerate(data, 1):
                        writer.writerow([ejecucion, idx, valor])
        generated_files.append(results_file)
        print(f"RESULTADOS guardados: {results_file.name}")

    return generated_files

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Extract execution results from files.')
    parser.add_argument('-p', '--programa', required=True, help='Path to the program to execute.')
    parser.add_argument('-n', '--n_value', required=True, type=int, help="Número de valores a generar")
    parser.add_argument('-s', '--seed', type=int, default=1234, help="Semilla para la generación de números aleatorios")
    parser.add_argument('-q', '--use_qemu', action='store_true', help="Ejecutar el programa con qemu-aarch64")
    parser.add_argument('-m', '--matrix', action='store_true', help="Modo matriz (n x n)")
    parser.add_argument('-o', '--origin', default=False, action='store_true', help="Se están extraendo resultados previos a la ejecución del programa")
    return parser.parse_args()

def main():
    """Main function that orchestrates the program execution."""
    args = parse_arguments()
    
    # Capturar resultados (ahora con ambos conjuntos de datos)
    results = medir_resultados(
        args.programa,
        args.n_value,
        args.seed,
        args.use_qemu,
        args.matrix,
        args.origin
    )

    if results:
        # Guardar en CSV (devuelve lista de archivos)
        generated_files = save_to_csv(
            results, 
            args.programa, 
            args.n_value, 
            args.seed, 
            args.matrix, 
            args.origin,
            args.use_qemu
        )
        
        # Mover archivos generados
        if generated_files:
            move_generated_files(generated_files, args.origin)
    else:
        print("No hay resultados para guardar.")

if __name__ == "__main__":
    main()