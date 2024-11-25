import numpy as np

def convolve1d(input_vector, kernel):
    vector_size = len(input_vector)
    kernel_size = len(kernel)
    temp_vector = np.zeros(vector_size)

    for i in range(vector_size):
        for j in range(kernel_size):
            if i + j < vector_size:
                temp_vector[i] += input_vector[i + j] * kernel[j]

    return temp_vector

def convolve1d_generic(input_vector, low_pass_kernel, high_pass_kernel):
    vector_size = len(input_vector)
    low_pass_result = np.zeros(vector_size)
    high_pass_result = np.zeros(vector_size)

    for i in range(vector_size):
        for j in range(len(low_pass_kernel)):
            if i + j < vector_size:
                print(f"low_pass_result[{i}] += input_vector[{i + j}] * low_pass_kernel[{j}] -> {input_vector[i + j]} * {low_pass_kernel[j]}")
                low_pass_result[i] += input_vector[i + j] * low_pass_kernel[j]

    for i in range(vector_size):
        for j in range(len(high_pass_kernel)):
            if i + j < vector_size:
                high_pass_result[i] += input_vector[i + j] * high_pass_kernel[j]

    output_vector = np.zeros(vector_size)
    for i in range(vector_size // 2):
        output_vector[i] = low_pass_result[2 * i]
        output_vector[vector_size // 2 + i] = high_pass_result[2 * i]

    print("low_pass_results = ", end="")
    for i in range(vector_size):
        print(f"{low_pass_result[i]:.6f} ", end="")
    print()

    print("high_pass_results = ", end="")
    for i in range(vector_size):
        print(f"{high_pass_result[i]:.6f} ", end="")
    print()

    return output_vector

if __name__ == "__main__":
    input_vector = np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0])

    low_pass_legall = np.array([-1.0/8, 1.0/4, 3.0/4, 1.0/4, -1.0/8])
    high_pass_legall = np.array([-1.0/2, 1.0, -1.0/2])

    low_pass_cdf97 = np.array([0.026748757411, -0.016864118443, -0.078223266529, 0.266864118443, 0.602949018236, 0.266864118443, -0.078223266529, -0.016864118443, 0.026748757411])
    high_pass_cdf97 = np.array([0.091271763114, -0.057543526229, -0.591271763114, 1.11508705, -0.591271763114, -0.057543526229, 0.091271763114])

    print("Convolving with LeGall 5/3 Wavelet")
    result_legall = convolve1d_generic(input_vector, low_pass_legall, high_pass_legall)
    print("Result:", result_legall)

    input_vector = np.array([1.0, 2.0, 3.0, 4.0, 5.0, 6.0])

    print("Convolving with CDF 9/7 Wavelet (lossy)")
    result_cdf97 = convolve1d_generic(input_vector, low_pass_cdf97, high_pass_cdf97)
    print("Result:", result_cdf97)
