## Compilación e información del tipo de dato float de 16 bits

### [Why is there no 2-byte float and does an implementation already exist?](https://stackoverflow.com/questions/5766882/why-is-there-no-2-byte-float-and-does-an-implementation-already-exist)
Discusión sobre la existencia y las implementaciones de floats de 16 bits en diferentes plataformas.

### [GCC Half-Precision](https://gcc.gnu.org/onlinedocs/gcc/Half-Precision.html)
Documentación oficial de GCC sobre la implementación de floats de media precisión.

### [Installing and Using GCC for ARM](https://learn.arm.com/install-guides/gcc/cross/)
Guía detallada sobre cómo instalar y usar GCC para cross-compilar para arquitecturas ARM.

### [How to Cross-Compile for ARM](https://askubuntu.com/questions/250696/how-to-cross-compile-for-arm)
Instrucciones sobre cómo cross-compilar programas para arquitecturas ARM en Ubuntu.

### [SPEC CPU 2017 Flags: GCC](https://www.spec.org/cpu2017/flags/gcc.2021-07-21.html#user_F-fomit-frame-pointer)
Descripciones de flags para GCC en SPEC CPU 2017, incluyendo optimizaciones y portabilidad, incluye información del flag `-fomit-frame-pointer`.

### [GCC Optimize Options](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Optimize-Options.html)
Opciones de compilación que controlan diversas optimizaciones del compilador (documentación oficial y más actualizada), incluyendo los flags `-fomit-frame-pointer` y `-ftree-vectorize`.

### [SPEC HPC 2021 Flags: GCC](https://www.spec.org/hpc2021/flags/gcc.html)
Información general sobre flags de compilación en la plataforma HPC de SPEC.

### [GCC Code Generation Options](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Code-Gen-Options.html#index-fPIC)
Opciones de compilación que controlan las convenciones de generación de código, incluyendo el flag `-fPIC`.

### [GCC x86 Options](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/x86-Options.html)
Opciones específicas de compilación para la familia de procesadores x86 (generación de instrucciones y ajuste de código para diferentes microarquitecturas), incluyendo los flags `-march` y `-mtune`.

### [BLAS Basic Linear Algebra Subprograms: cblas_hgemm](https://developer.arm.com/documentation/101004/2410/BLAS-Basic-Linear-Algebra-Subprograms/CBLAS-functions/cblas-hgemm?lang=en)
Información sobre la función `cblas_hgemm` en BLAS, que realiza multiplicaciones de matrices con números en formato float de 16 bits.

### [Arm Performance Libraries (ARMPL)](https://learn.arm.com/install-guides/armpl/)
Guía de instalación y uso de las bibliotecas de alto rendimiento de ARM (ARMPL), diseñadas para optimizar operaciones matemáticas y científicas en sistemas basados en arquitecturas ARM.

### [Advanced Matrix Extensions (AMX)](https://en.wikipedia.org/wiki/Advanced_Matrix_Extensions)
Información sobre las Advanced Matrix Extensions (AMX), una tecnología de hardware desarrollada por Intel para acelerar operaciones matriciales en procesadores modernos, ampliamente utilizada en aplicaciones de aprendizaje automático y computación de alto rendimiento.

### [Intel Intrinsics Guide BF16 Instructions](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/intrinsics-for-avx-512-bf16-instructions.html)
Guía de intrínsecos de Intel para instrucciones BF16, que proporciona una referencia rápida para el uso de estas instrucciones en código C/C++.

### [Half-precision floating-point format](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
Información sobre los tipos de datos flotantes de media precisión, incluyendo información como el exponente mínimo y máximo posible (en base 10)

### [bfloat16 floating-point format](https://en.wikipedia.org/wiki/Bfloat16_floating-point_format)
Información sobre los tipos de datos flotantes bfloat16, incluyendo información como el exponente mínimo y máximo posible (en base 10). Contiene además una comparativa gráfica entre los distintos formatos de almacenamiento.

### [GCC Predefined Macros: __FLT16_ and __BFLT16_](https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html)
Se han consultado las macros de GCC para _Float16 y bfloat16 mediante el siguiente comando: 
```bash
{ echo | gcc -dM -E - | grep '^#define __FLT16_'; echo | gcc -dM -E - | grep '^#define __BFLT16_'; }
```

## Soporte de hardware

### [Half-precision floating-point format](https://en.wikipedia.org/wiki/Half-precision_floating-point_format)
Descripción detallada del formato de punto flotante de media precisión y su soporte en varias arquitecturas.

### [Mixed-Precision Programming with CUDA 8](https://developer.nvidia.com/blog/mixed-precision-programming-cuda-8/)
Introducción a la programación de precisión mixta con CUDA 8, incluyendo ejemplos y beneficios.

### [Advanced SIMD (Neon) intrinsics Introduction](https://developer.arm.com/documentation/101028/0010/Advanced-SIMD--Neon--intrinsics)
Guía sobre el uso de intrinsics SIMD avanzados (Neon) en la arquitectura Armv8.2-A.

### [ROCm Precision Support](https://rocm.docs.amd.com/en/latest/reference/precision-support.html#floating-point-types)
Documentación sobre el soporte de diferentes tipos de precisión en la plataforma ROCm de AMD, incluyendo float16 y bfloat16, así como su compatibilidad con diversas arquitecturas de hardware.

### [4th Gen AMD EPYC Processor Architecture - White Paper](https://www.amd.com/content/dam/amd/en/documents/epyc-business-docs/white-papers/221704010-B_en_4th-Gen-AMD-EPYC-Processor-Architecture---White-Paper_pdf.pdf)
Documento que confirma la introducción de las instrucciones AVX-512 y el soporte para BFLOAT16 en la cuarta generación de procesadores AMD EPYC que implementan la arquitectura Zen 4.

### [LoongArch Documentation](https://github.com/loongson/LoongArch-Documentation/releases/latest/download/LoongArch-Vol1-v1.10-EN.pdf)
Documentación oficial de LoongArch v1.1, detallando las especificaciones y soporte para, entre otras, instrucciones .H que operan con datos de 16 bits, enteros y floats (FP16). "If operand is a floating-point number, the suffixes of the instruction name are .H (half precision)" 

### [AMD Zen 5 y Zen 6: AVX512-VP2INTERSECT y AVX512-FP16](https://elchapuzasinformatico.com/2024/01/amd-cpu-zen-5-avx512-vp2intersect-zen-6-avx512-fp16/)
Descripción de las nuevas instrucciones AVX512-VP2INTERSECT y AVX512-FP16 en las arquitecturas AMD Zen 5 y Zen 6, respectivamente. También aporta información sobre el soporte de operaciones BF16 en AMD Zen 4 y algunas arquitecturas de Intel.

### [AMD Zen 5 Compiler Support Posted For GCC - Confirms New AVX Features & More](https://www.phoronix.com/news/AMD-Zen-5-Znver-5-GCC)
Confirmación de que AMD Zen 5 tendrá (tiene) soporte para extensiones AVX512BF16.

### [Huawei's Ascend 910C: A Bold Challenge to Nvidia in the AI Chip Market](https://www.unite.ai/huaweis-ascend-910c-a-bold-challenge-to-nvidia-in-the-ai-chip-market/)
Artículo que destaca cómo el procesador Huawei Ascend 910C soporta operaciones de float de 16 bits (FP16) de manera nativa, presentándose como un fuerte competidor en el mercado de chips para inteligencia artificial frente a Nvidia.

### [HiSilicon - Processors](https://en.wikipedia.org/wiki/HiSilicon#Server_processors)
Información sobre los procesadores de servidor de HiSilicon y los chips Ascend que soportan operaciones FP16 de manera nativa, así como detalles sobre la capacidad de emulación de operaciones FP16 en otros procesadores como el Kunpeng.

### [ArchitectureSpecificsMemo](https://wiki.debian.org/ArchitectureSpecificsMemo)
Página del wiki de Debian que proporciona una referencia rápida de las especificaciones de diferentes arquitecturas, incluyendo los resultados del comando `uname -m` para cada arquitectura.

### [RISC-V ISA Manual - zfh](https://www.five-embeddev.com/riscv-user-isa-manual/latest-adoc/zfh.html)
Información sobre el soporte de instrucciones para operaciones de punto flotante de 16 bits (FP16) en la arquitectura RISC-V a través de la extensión zfh.

### [RISC-V BF16 Extensions](https://mirror.iscas.ac.cn/riscv-toolchains/release/riscv/riscv-bfloat16/LatestRelease/riscv-bfloat16.pdf)
Documento que detalla las extensiones BF16 para RISC-V.

### [LLVM Clang Support for AVX512FP16](https://www.phoronix.com/news/LLVM-Clang-AVX512FP16)
Confirmación de que LLVM Clang tiene soporte para la extensión AVX512FP16, permitiendo operaciones de punto flotante de 16 bits en arquitecturas compatibles desde la versión 14.0.

## Otros

### [Source code lapack](https://github.com/Reference-LAPACK/lapack)

### [lapack ssyev](https://www.math.utah.edu/software/lapack/lapack-s/ssyev.html)
Condiciones de contorno para la función `ssyev` de LAPACK, destacando la condición para el argumento `lwork` que debe ser por lo menos igual a `3 * n - 1`.

### [Arithmetic Types in C](https://en.cppreference.com/w/c/language/arithmetic_types.html)
Definición de los tipos aritméticos incluidos en la definición de C, incluyendo los tipos de punto flotante y sus especificaciones.

### [ARMv8.2-A](https://es.wikipedia.org/wiki/AArch64#ARMv8.2-A)