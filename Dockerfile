# Use Ubuntu 24.04 as base image
FROM ubuntu:24.04

# Install dependencies for building and compiling
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    gcc-14 \
    g++-14 \
    valgrind \
    qemu-user \
    python3 \
    python3-pip \
    python3-venv \
    libgsl-dev \
    wget \
    cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Set up GCC 14 as default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

# Install ARM toolchain conditionally based on architecture
RUN if [ "$(uname -m)" = "x86_64" ] || [ "$(uname -m)" = "amd64" ]; then \
        apt-get update && \
        apt-get install -y --no-install-recommends \
        gcc-aarch64-linux-gnu \
        g++-aarch64-linux-gnu && \
        apt-get clean && \
        rm -rf /var/lib/apt/lists/*; \
    else \
        echo "Not installing ARM cross-compiler as the architecture is not x86_64 or amd64"; \
    fi

# Copy the requirements file and the script from the host to the container
COPY ./Tests-Python/requirements.txt /tmp/requirements.txt
# Empleamos el script setup-python-env.sh para crear un entorno virtual porque falla al instalar las dependencias en el entorno global
COPY ./Tests-Python/setup-python-env.sh /tmp/setup-python-env.sh

# Give execution permission to the script
RUN chmod +x /tmp/setup-python-env.sh

# Run the script to create the virtual environment and install packages
RUN /tmp/setup-python-env.sh

# Set the default working directory
WORKDIR /workspace

# Set entrypoint to bash
CMD ["/bin/bash"]
