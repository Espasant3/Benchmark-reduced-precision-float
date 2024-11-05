import numpy as np

# Definir el tamaño de la señal
N = 8
M_PI = 3.14159265358979323846

def dct(input):
    output = np.zeros(N)
    for k in range(N):
        if k == 0:
            alpha = np.sqrt(1.0 / N)
        else:
            alpha = np.sqrt(2.0 / N)
        sum = 0.0
        for n in range(N):
            sum += input[n] * np.cos(M_PI * (2.0 * n + 1.0) * k / (2.0 * N))
        output[k] = alpha * sum
    return output

def main():
    input = np.array([1.0, 2.0, 3.0, 4.0, 4.0, 3.0, 2.0, 1.0], dtype=float)
    
    # Si necesitas valores aleatorios para el input, descomenta estas líneas:
    # np.random.seed(seed)
    # input = np.random.rand(N) * 10.0
    
    output = dct(input)
    print("DCT:")
    for val in output:
        print(f"{val:.6f}")

if __name__ == "__main__":
    main()
