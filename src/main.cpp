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

  std::cout << "STARTING BRANCH TEST..." << std::endl;

  // 1. addi x1, x0, 5    (Counter = 5)
  //    Hex: 0x00500093
  ram.write(0, 0x93);
  ram.write(1, 0x00);
  ram.write(2, 0x50);
  ram.write(3, 0x00);

  // 2. addi x2, x0, 1    (Decrementer = 1)
  //    Hex: 0x00100113
  ram.write(4, 0x13);
  ram.write(5, 0x01);
  ram.write(6, 0x10);
  ram.write(7, 0x00);

  // LABEL: LOOP_START (Address 8)
  // 3. sub x1, x1, x2    (x1 = x1 - 1)
  //    Hex: 0x402080B3
  ram.write(8, 0xB3);
  ram.write(9, 0x80);
  ram.write(10, 0x20);
  ram.write(11, 0x40);

  // 4. bne x1, x0, -4    (If x1 != 0, jump back to 8)
  //    Offset = -4 (0xFFFFFFFC). B-Type encoding is complex, trust the hex!
  //    Hex: 0xFE009EE3
  ram.write(12, 0xE3);
  ram.write(13, 0x9E);
  ram.write(14, 0x00);
  ram.write(15, 0xFE);

  // --- PROGRAM END ---

  // Run enough cycles for the loop to finish (5 iterations * 2 instructions +
  // setup)
  for (int i = 0; i < 20; ++i) {
    // Break if we go past the code
    if (cpu.get_pc() >= 16)
      break;
    cpu.step();
  }

  cpu.dump_registers();

  return 0;
}
