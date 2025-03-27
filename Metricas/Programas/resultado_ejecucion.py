import os
import re
import csv
import argparse
from pathlib import Path
import subprocess

def medir_resultados(programa, n, seed, use_qemu, matrix_mode):
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
        resultados = []
        lines = resultado.stdout.split('\n')
        i = 0
        while i < len(lines):
            linea = lines[i]
            if "Resultados ejecucion:" in linea:
                if matrix_mode:
                    # Modo matriz: capturar todas las filas que empiezan con tabulación
                    i += 1  # Saltar la línea "Resultados ejecucion:"
                    valores = []
                    while i < len(lines) and lines[i].startswith('\t'):
                        # Extraer valores de la fila (eliminar tabulador y espacios)
                        fila = lines[i].strip().split()
                        valores.extend([float(v) for v in fila])
                        i += 1
                    # Validar total de elementos = n x n
                    if len(valores) != n * n:
                        print(f"Error en matriz: {len(valores)} elementos (esperados: {n*n})")
                        return None
                    resultados.append(valores)
                else:
                    # Modo array: capturar una línea de resultados
                    valores = linea.split(":")[1].strip().split()
                    if len(valores) != n:
                        print(f"Error en array: {len(valores)} elementos (esperados: {n})")
                        return None
                    resultados.append([float(v) for v in valores])
                    i += 1
            else:
                i += 1
        
        if resultados:
            print(f"n={n}: {len(resultados)} ejecuciones capturadas")
            return resultados
        else:
            print("No se encontraron resultados.")
            return None
            
    except Exception as e:
        print(f"Error en n={n}: {e}")
        return None


def save_to_csv(results, programa, n, seed, matrix_mode):
    """Guarda resultados en formato CSV según el modo (array/matriz)."""
    script_dir = Path(__file__).parent
    programa_nombre = Path(programa).stem
    output_file = script_dir / f"{programa_nombre}_n{n}_seed{seed}.csv"
    
    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        if matrix_mode:
            # Modo matriz: Ejecucion, Fila, Columna, Valor
            writer.writerow(["Ejecucion", "Fila", "Columna", "Valor"])
            for ejecucion, ejec_data in enumerate(results, start=1):
                # Iterar sobre cada fila y columna de la matriz
                idx = 0
                for fila in range(n):
                    for columna in range(n):
                        valor = ejec_data[idx]
                        writer.writerow([ejecucion, fila + 1, columna + 1, valor])  # +1 para índices base 1
                        idx += 1
        else:
            # Modo array: Ejecucion, Indice, Valor
            writer.writerow(["Ejecucion", "Indice", "Valor"])
            for ejecucion, ejec_data in enumerate(results, start=1):
                for indice, valor in enumerate(ejec_data, start=1):
                    writer.writerow([ejecucion, indice, valor])
    
    print(f"Archivo guardado: {output_file}")

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='Extract execution results from files.')
    parser.add_argument('-p', '--programa', required=True, help='Path to the program to execute.')
    parser.add_argument('-n', '--n_value', required=True, type=int, help="Número de valores a generar")
    parser.add_argument('-s', '--seed', type=int, default=1234, help="Semilla para la generación de números aleatorios")
    parser.add_argument('-q', '--use_qemu', action='store_true', help="Ejecutar el programa con qemu-aarch64")
    parser.add_argument('-m', '--matrix', action='store_true', help="Modo matriz (n x n)")
    return parser.parse_args()

def main():
    """Main function that orchestrates the program execution."""
    args = parse_arguments()
    
    all_results = medir_resultados(
        args.programa,
        args.n_value,
        args.seed,
        args.use_qemu,
        args.matrix
    )

    if all_results:
        save_to_csv(all_results, args.programa, args.n_value, args.seed, args.matrix)
    else:
        print("No results to save.")

if __name__ == "__main__":
    main()