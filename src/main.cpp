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

  std::cout << "STARTING FUNCTION CALL TEST..." << std::endl;

  // --- PROGRAM: "The Function Call" ---

  // 0. jal x1, 12      (Call function at addr 12. Save Return Addr to x1)
  //    Offset = 12. Hex: 0x00C000EF
  ram.write(0, 0xEF);
  ram.write(1, 0x00);
  ram.write(2, 0xC0);
  ram.write(3, 0x00);

  // 4. addi x2, x0, 99 (Return Point! We should see x2 = 99 at the very end)
  //    Hex: 0x06300113
  ram.write(4, 0x13);
  ram.write(5, 0x01);
  ram.write(6, 0x30);
  ram.write(7, 0x06);

  // 8. Infinite Loop (Stop here)
  //    beq x0, x0, 0 (Jump to self) -> Hex: 0x00000063
  ram.write(8, 0x63);
  ram.write(9, 0x00);
  ram.write(10, 0x00);
  ram.write(11, 0x00);

  // --- THE FUNCTION (Address 12) ---

  // 12. addi x3, x0, 77 (Do work inside function)
  //     Hex: 0x04D00193
  ram.write(12, 0x93);
  ram.write(13, 0x01);
  ram.write(14, 0xD0);
  ram.write(15, 0x04);

  // 16. jalr x0, 0(x1)  (Return! Jump to address in x1)
  //     Hex: 0x00008067
  ram.write(16, 0x67);
  ram.write(17, 0x80);
  ram.write(18, 0x00);
  ram.write(19, 0x00);

  for (int i = 0; i < 10; ++i) {
    // Break if we go past the code
    if (cpu.get_pc() == 8 && i > 0)
      break;
    cpu.step();
  }

  cpu.dump_registers();

  return 0;
}
