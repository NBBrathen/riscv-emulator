#include "cpu.h"
#include "memory.h"
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./riscv <binary_file>" << std::endl;
    return 1;
  }

  // 1MB of RAm
  Memory ram(1024 * 1024);

  // Load the binary file specified in the argument
  if (!ram.load_binary(argv[1])) {
    return 1;
  }

  CPU cpu(ram);

  // Starting the emulator
  std::cout << "STARTING EMULATOR...\n=====================" << "\n";

  // Uncomment to print the size of the memory for testing
  // std::cout << "Memory contains " << ram.get_size() << " bytes" << std::endl;

  // Run
  try {
    for (int i = 0; i < 1000; ++i) {
      // Break if we hit a stop instruction (like 0x00000000)
      if (ram.read(cpu.get_pc()) == 0)
        break;

      cpu.step();
    }
  } catch (const std::exception &e) {
    std::cerr << "CPU Exception: " << e.what() << std::endl;
  }

  cpu.dump_registers();

  std::cout << "\nSTARTING BENCHMARK...\n=====================" << std::endl;
  ram.write(0, 0x6F);
  ram.write(1, 0x00);
  ram.write(2, 0x00);
  ram.write(3, 0x00);

  CPU bench_cpu(ram);

  auto start = std::chrono::high_resolution_clock::now();
  uint64_t cycles = 0;

  while (true) {
    bench_cpu.step();
    cycles++;

    if (cycles % 100000 == 0) {
      auto now = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> diff = now - start;
      if (diff.count() >= 1.0)
        break; // stop after 1 second
    }
  }

  double mips = cycles / 1000000.0;
  std::cout << "---------------------------------------------------------------"
            << std::endl;
  std::cout << "Performance: " << mips << " MIPS" << std::endl;
  std::cout << "---------------------------------------------------------------"
            << std::endl;

  return 0;
}
