import numpy as np
import argparse
import time

M_PI = 3.14159265358979323846

def positive_int(value):
    ivalue = int(value)
    if ivalue < 0:
        raise argparse.ArgumentTypeError(f"Negative value not allowed: {ivalue}")
    return ivalue

def dct(input, N):
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
    parser = argparse.ArgumentParser(description="Perform the DCT (discrete cosine transform) operation on a signal.")
    parser.add_argument("n", type=int, help="Vector size", metavar="N")
    parser.add_argument("-s", "--seed", type=positive_int, help="Random seed (optional, must be non-negative)", default=None, metavar="SEED")
    args = parser.parse_args()

    n = args.n
    
    if args.seed is not None:
        np.random.seed(args.seed)
    else:
        np.random.seed(int(time.time()))
    
    # Example small array DCT calculation to match your C code
    
    N_SMALL = 5
    input_small = np.random.rand(N_SMALL) * 10.0
    output_small = dct(input_small, N_SMALL)
    print(f"Array input_small: {input_small}")
    print(f"DCT_small: {output_small}")

    input = np.random.rand(n) * 10.0
    
    print(f"Array input: {input}")
    
    output = dct(input, n)
    print(f"DCT: {output}")
        

if __name__ == "__main__":
    main()
