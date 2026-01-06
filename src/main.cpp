#include "cpu.h"
#include "memory.h"
#include <iostream>

int main() {
  // 1MB of RAm
  Memory ram(1024 * 1024);

  CPU cpu(ram);

  // Starting the emulator
  std::cout << "STARTING EMULATOR...\n=====================" << "\n";

  // Printing the size of the memory for testing
  // std::cout << "Memory contains " << ram.get_size() << " bytes" << std::endl;

  // Manually injecting code to test cpu fetch and step
  ram.write(0, 0x93);
  ram.write(1, 0x00);
  ram.write(2, 0x20);
  ram.write(3, 0x03);

  std::cout << "MEMORY INITIALIZED. RUNNING 1 CYCLE..." << std::endl;
  cpu.step();

  cpu.dump_registers();

  return 0;
}
