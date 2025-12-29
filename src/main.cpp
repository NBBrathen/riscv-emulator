#include "cpu.h"
#include "memory.h"
#include <iostream>

int main() {
  // 1MB of RAm
  Memory ram(1024 * 1024);

  CPU cpu(ram);

  std::cout << "STARTING EMULATOR...\n=====================" << "\n";
  std::cout << "Memory contains " << ram.get_size() << " bytes" << std::endl;

  return 0;
}
