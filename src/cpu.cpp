#include "cpu.h"
#include <cstdint>
#include <iomanip>
#include <iostream>

CPU::CPU(Memory &memory) : mem(memory) {
  pc = 0;
  regs.fill(0);
}

void CPU::step() {
  // First, fetch instruction from current pc
  uint32_t instruction = fetch();

  // Next, advance PC to next instruction (4 bytes forward as RISC-V is 32 bits
  // per instruction)
  pc += 4;

  // Finally, execute the instruction
  execute(instruction);

  // x0 is read-only, so set it to 0 if it was changed.
  regs[0] = 0;
}

void CPU::dump_registers() const {
  std::cout << "---------------------------------------------------------------"
               "-----------------------\n   RISC-V CORE "
               "DUMP\n---------------------------------------------------------"
               "-----------------------------"
            << std::endl;

  std::cout << "   PC : " << "0x" << std::hex << std::setw(8)
            << std::setfill('0') << pc << std::dec << "\n\n";

  for (int i = 0; i < 32; i++) {
    std::cout << "   x" << std::dec << i << " : 0x";

    std::cout << std::hex << std::setw(8) << std::setfill('0') << regs[i];

    if ((i + 1) % 4 == 0) {
      std::cout << "\n";
    } else {
      std::cout << "   ";
    }
  }
}

uint32_t CPU::fetch() {
  // Read the 4 bytes from memory
  uint32_t inst = 0;
  inst |= mem.read(pc);             // Byte 1 (LSB)
  inst |= (mem.read(pc + 1) << 8);  // Byte 2
  inst |= (mem.read(pc + 2) << 16); // Byte 3
  inst |= (mem.read(pc + 3) << 24); // Byte 4 (MSB)

  return inst;
}

void CPU::execute(uint32_t instruction) {
  uint32_t opcode = instruction & 0x7F;
  switch (opcode) {
  case 0x13: { // ADDI, ORI, ANDI (Immediate math)
    std::cout << "Executing: ADDI (Opcode 0x13)" << std::endl;
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (int32_t)(instruction >> 20);

    switch (funct3) {
    case 0x0: // ADDI
      std::cout << "ADDI: x" << rd << " = x" << rs1 << " + " << imm
                << std::endl;
      regs[rd] = regs[rs1] + imm;
      break;

    case 0x6: // ORI
      std::cout << "ORI" << std::endl;
      break;

    case 0x7: // ANDI
      std::cout << "ANDI" << std::endl;
      break;

    default:
      std::cout << "Unknown I-Type funct3: " << funct3 << std::endl;
      break;
    }
    break;
  }
  case 0x33:
    std::cout << "Executing: R-Type (Opcode 0x33)" << std::endl;
    break;
  default:
    std::cerr << "Unknown Opcode: 0x" << std::hex << opcode << std::endl;
    break;
  }
}
