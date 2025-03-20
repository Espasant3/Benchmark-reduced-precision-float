import os
import re
import csv
import argparse
from pathlib import Path
import subprocess

def medir_resultados(programa, n, seed, use_qemu):
    try:
        # Determinar el comando a ejecutar
        if use_qemu:
            comando = ["qemu-aarch64", programa, str(n), str(seed), "-v"]
        else:
            comando = [programa, str(n), str(seed), "-v"]
        
        # Ejecutar el programa una única vez
        resultado = subprocess.run(
            comando,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        
        # Capturar todas las líneas de resultados
        resultados = []
        for linea in resultado.stdout.split('\n'):
            if "Resultados ejecucion:" in linea:
                valores = linea.split(":")[1].strip().split()
                resultados.append([float(valor) for valor in valores])  # Lista de listas
        
        # Validar que todas las líneas tengan 'n' resultados
        if all(len(linea) == n for linea in resultados) and resultados:
            print(f"n={n}: {len(resultados)} ejecuciones capturadas")
            return resultados
        else:
            print(f"Error: Número de resultados incorrecto en alguna ejecución")
            print(f"n={n}: {len(resultados)} ejecuciones capturadas")
            return None
            
    except Exception as e:
        print(f"Error en n={n}: {e}")
        return None

def save_to_csv(results, programa, n, seed):
    """Save results to a CSV file in the script's directory."""
    # Obtener el directorio del script actual
    script_dir = Path(__file__).parent
    
    # Generar el nombre del archivo basado en el programa, n y seed
    programa_nombre = Path(programa).stem  # Obtener el nombre del programa sin extensión
    output_file = script_dir / f"{programa_nombre}_n{n}_seed{seed}.csv"
    
    # Guardar los resultados en el archivo CSV
    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Ejecucion", "Indice", "Valor"])  # Encabezados
        
        # Escribir cada valor con su número de ejecución e índice
        for ejecucion, linea_resultados in enumerate(results, start=1):
            for indice, valor in enumerate(linea_resultados, start=1):
                writer.writerow([ejecucion, indice, valor])
    
    print(f"Results saved to {output_file}")

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Extract execution results from files.')
    parser.add_argument('-p', '--programa', required=True, help='Path to the program to execute.')
    parser.add_argument('-n', '--n_value', required=True, type=int, help="Número de valores a generar")
    parser.add_argument('-s', '--seed', type=int, default=1234, help="Semilla para la generación de números aleatorios")
    parser.add_argument('-q', '--use_qemu', action='store_true', help="Ejecutar el programa con qemu-aarch64")
    return parser.parse_args()

def main():
    """Main function that orchestrates the program execution."""
    args = parse_arguments()
    
    all_results = medir_resultados(args.programa, args.n_value, args.seed, args.use_qemu)

    if all_results:
        save_to_csv(all_results, args.programa, args.n_value, args.seed)
    else:
        print("No results to save.")

if __name__ == "__main__":
    main()