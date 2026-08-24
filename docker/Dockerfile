FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update && apt-get install -y \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    wget \
    nasm \
    xorriso \
    mtools \
    grub-pc-bin \
    grub-common \
    qemu-system-x86 \
    gdb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/mini-os

COPY scripts/build_cross_compiler.sh /root/build_cross_compiler.sh
RUN chmod +x /root/build_cross_compiler.sh && /root/build_cross_compiler.sh

ENV PATH="/root/opt/cross/bin:${PATH}"