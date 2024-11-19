## Compilación e información del tipo de dato float de 16 bits

### [Why is there no 2-byte float and does an implementation already exist?](https://stackoverflow.com/questions/5766882/why-is-there-no-2-byte-float-and-does-an-implementation-already-exist)
Discusión sobre la existencia y las implementaciones de floats de 16 bits en diferentes plataformas.

### [GCC Half-Precision](https://gcc.gnu.org/onlinedocs/gcc/Half-Precision.html)
Documentación oficial de GCC sobre la implementación de floats de media precisión.

### [GCC 4.9.4 Half-Precision](https://gcc.gnu.org/onlinedocs/gcc-4.9.4/gcc/Half-Precision.html)
Documentación de GCC versión 4.9.4 sobre floats de media precisión.

### [Installing and Using GCC for ARM](https://learn.arm.com/install-guides/gcc/cross/)
Guía detallada sobre cómo instalar y usar GCC para cross-compilar para arquitecturas ARM.

### [How to Cross-Compile for ARM](https://askubuntu.com/questions/250696/how-to-cross-compile-for-arm)
Instrucciones sobre cómo cross-compilar programas para arquitecturas ARM en Ubuntu.

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
Documentación oficial de LoongArch v1.1, detallando las especificaciones y soporte para, entre otras, instrucciones .H que operan con datos de 16 bits, enteros y floats (FP16).

### [AMD Zen 5 y Zen 6: AVX512-VP2INTERSECT y AVX512-FP16](https://elchapuzasinformatico.com/2024/01/amd-cpu-zen-5-avx512-vp2intersect-zen-6-avx512-fp16/)
Descripción de las nuevas instrucciones AVX512-VP2INTERSECT y AVX512-FP16 en las arquitecturas AMD Zen 5 y Zen 6, respectivamente.

### [Huawei's Ascend 910C: A Bold Challenge to Nvidia in the AI Chip Market](https://www.unite.ai/huaweis-ascend-910c-a-bold-challenge-to-nvidia-in-the-ai-chip-market/)
Artículo que destaca cómo el procesador Huawei Ascend 910C soporta operaciones de float de 16 bits (FP16) de manera nativa, presentándose como un fuerte competidor en el mercado de chips para inteligencia artificial frente a Nvidia.

### [HiSilicon - Processors](https://en.wikipedia.org/wiki/HiSilicon#Server_processors)
Información sobre los procesadores de servidor de HiSilicon y los chips Ascend que soportan operaciones FP16 de manera nativa, así como detalles sobre la capacidad de emulación de operaciones FP16 en otros procesadores como el Kunpeng.

### [ArchitectureSpecificsMemo](https://wiki.debian.org/ArchitectureSpecificsMemo)
Página del wiki de Debian que proporciona una referencia rápida de las especificaciones de diferentes arquitecturas, incluyendo los resultados del comando `uname -m` para cada arquitectura.
