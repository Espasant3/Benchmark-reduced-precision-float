import numpy as np
import argparse
import time

def positive_int(value):
    ivalue = int(value)
    if ivalue < 0:
        raise argparse.ArgumentTypeError(f"Valor negativo no permitido: {ivalue}")
    return ivalue

def axpy(n, a, x, y):
    for i in range(n):
        y[i] = a * x[i] + y[i]
    return y

def main():
    parser = argparse.ArgumentParser(description="Perform axpy operation.")
    parser.add_argument("n", type=int, help="Size of the vectors", metavar="N")
    parser.add_argument("-s", "--seed", type=positive_int, help="Random seed (optional, must be non-negative)", default=None, metavar="SEED")
    args = parser.parse_args()

    n = args.n
    a = 2.3752

    if args.seed is not None:
        np.random.seed(args.seed)
    else:
        np.random.seed(int(time.time()))

    x_small = np.random.rand(n) * 10.0
    y_small = np.random.rand(n) * 10.0

    print(f"Array x_small: {x_small}")
    print(f"Array y_small: {y_small}")

    y_small = axpy(n, a, x_small, y_small)

    print(f"Array y_small despues de AXPY: {y_small}")

if __name__ == "__main__":
    main()
