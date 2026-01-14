# RISC-V RV32I Emulator

A high-performance, cycle-accurate emulator for the RISC-V 32-bit Integer (RV32I) instruction set architecture, written in modern C++.

## Key Features

* **Core Architecture**: Implements the complete unprivileged RV32I Base Integer ISA including branching, memory operations, and ALU processing.
* **High Performance**: Optimized execution pipeline achieving ~208 MIPS (Million Instructions Per Second) on standard hardware via `-O3` optimization and fast-path memory access.
* **System Simulation**:
   * 32-bit Physical Address Space simulating a little-endian memory controller.
   * Binary Loader for executing raw machine code files.
   * Bare Metal Support: Automatic initialization of Stack Pointer (SP) and Return Address (RA) for standalone execution without an OS.
* **Toolchain Compatible**: Fully capable of running C programs compiled with the standard RISC-V GCC Toolchain.

## Project Structure

```
.
├── src/
│   ├── cpu.cpp       # Instruction fetch/decode/execute logic
│   ├── memory.cpp    # 1MB RAM simulation and binary loading
│   └── main.cpp      # Entry point and benchmark harness
├── asm_to_bin.py     # Python utility to create simple binary test files
├── main.c            # Example C program for bare-metal compilation
├── CMakeLists.txt    # Build configuration
└── README.md         # Project documentation
```

## Benchmarks

| Metric | Result | Notes |
|--------|--------|-------|
| Throughput | 208 MIPS | Measured on host machine (Infinite Loop Test) |
| ISA Coverage | 100% (RV32I) | Supports R, I, S, B, U, and J instruction formats |
| Memory Size | 1MB | Configurable in `src/main.cpp` |

## Building & Running

### Prerequisites

* CMake (3.15+)
* C++ Compiler (GCC/Clang) supporting C++20
* RISC-V Toolchain (Optional, only needed to compile new C programs)
   * Arch Linux: `sudo pacman -S riscv64-elf-gcc riscv64-elf-newlib`
   * Ubuntu: `sudo apt install gcc-riscv64-unknown-elf`

### Build Instructions

```bash
mkdir build && cd build
cmake ..
make
```

### Usage

Run a raw binary file:

```bash
./riscv path/to/program.bin
```

Generate a test binary: Included is a Python script to assemble simple instructions into a binary file for testing.

```bash
python3 ../asm_to_bin.py
./riscv test.bin
```

## Running C Programs

This emulator runs "Bare Metal" code. To compile a standard C file for it, you must use specific flags to disable standard library startup files.

1. Create a C file (`main.c`):

```c
int main() {
    int a = 10;
    int b = 32;
    return a + b; // Result (42) will be in register x10 (a0)
}
```

2. Compile and Extract Binary:

```bash
# Compile to ELF
riscv64-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -Ttext=0x0 -o main.elf main.c

# Extract raw machine code to .bin
riscv64-elf-objcopy -O binary main.elf main.bin
```

3. Run in Emulator:

```bash
./riscv main.bin
```

4. Check Output: The emulator dumps the register state upon completion. Look for `x10` (the return value register):

```
---------------------------------------------------------------
   RISC-V CORE DUMP
---------------------------------------------------------------
   PC : 0xfffffffe

   x0 : 0x00000000       x1 : 0xffffffff       x2 : 0x00100000
   ...
   x10 : 0x0000002a      ...
```

(Note: 0x2a is 42 in decimal)

## Technical Architecture

### Memory Map

The emulator provides a flat 1MB RAM space starting at address `0x00000000`.

* **Code Segment**: Loaded at `0x0`.
* **Stack Pointer (x2)**: Initialized to the top of memory (`0x00100000`) and grows downwards.

### The "Magic Exit"

Since there is no Operating System to handle program termination, the emulator creates a virtual "Exit Trap."

1. Register `x1` (Return Address) is initialized to `0xFFFFFFFF`.
2. When `main()` returns, it executes `ret` (which is `jalr x0, 0(x1)`).
3. The CPU `fetch()` cycle detects this specific address and cleanly halts execution before reading invalid memory.

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Write tests for new functionality
4. Ensure all tests pass (`make test`)
5. Submit a pull request with a clear description of changes

## License

This project is licensed under the MIT License - see the LICENSE.txt file for details.
