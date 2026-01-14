# RISC-V RV32I Emulator

A high-performance, cycle-accurate emulator for the **RISC-V 32-bit Integer (RV32I)** instruction set architecture, written in modern C++.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)

## Key Features

* **Core Architecture**: Implements the complete unprivileged **RV32I Base Integer ISA** (40+ instructions).
* **High Performance**: Optimized fetch-decode-execute pipeline achieving **>200 MIPS** (Million Instructions Per Second) on standard hardware.
* **System Simulation**:
    * **32-bit Physical Address Space** with little-endian memory controller.
    * **Binary Loader** for raw machine code execution.
    * **Magic Exit Trap** for clean termination of bare-metal programs.
* **Toolchain Compatible**: Fully capable of running C programs compiled with the **RISC-V GCC Toolchain**.

## Building & Running

### Prerequisites
* CMake (3.10+)
* C++ Compiler (GCC/Clang) supporting C++17
* RISC-V Toolchain (optional, for compiling C tests)

### Build
```bash
mkdir build && cd build
cmake ..
make
