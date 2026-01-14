#include "cpu.h"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>

CPU::CPU(Memory &memory) : mem(memory) {
  pc = 0;
  regs.fill(0);

  // The stack grows downwards, so we start at the end
  regs[2] = mem.get_size();
  regs[1] = 0xFFFFFFFF; // Return Address (Exit Code)
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

  // Check for Exit Address
  if ((pc & 0xFFFFFFFE) == 0xFFFFFFFE) {
    throw std::runtime_error("Program finished.");
  }

  return mem.read_word(pc);
}

void CPU::execute(uint32_t instruction) {
  uint32_t opcode = instruction & 0x7F;
  switch (opcode) {
  case 0x13: { // ADDI, ORI, ANDI (Immediate math)
    std::cout << "Executing: ADDI (Opcode 0x13)" << std::endl;
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (int32_t)(instruction & 0xFFF00000) >> 20;

    switch (funct3) {
    case 0x0: // ADDI
      std::cout << "ADDI: x" << rd << " = x" << rs1 << " + " << imm
                << std::endl;
      regs[rd] = regs[rs1] + imm;
      break;

    case 0x6: // ORI
      std::cout << "ORI: x" << rd << " = x" << rs1 << " | " << imm << std::endl;
      regs[rd] = regs[rs1] | imm;

      break;

    case 0x7: // ANDI
      std::cout << "ANDI: x" << rd << " = x" << rs1 << " & " << imm
                << std::endl;
      regs[rd] = regs[rs1] & imm;
      break;

    case 0x4:
      std::cout << "XORI: x" << rd << " = x" << rs1 << " ^ " << imm
                << std::endl;
      regs[rd] = regs[rs1] ^ imm;
      break;

    default:
      std::cout << "Unknown I-Type funct3: " << funct3 << std::endl;
      break;
    }
    break;
  }
  case 0x33: {
    std::cout << "Executing: R-Type (Opcode 0x33)" << std::endl;
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t rs2 = (instruction >> 20) & 0x1F;
    uint32_t funct7 = (instruction >> 25) & 0x7F;

    switch (funct3) {
    case 0x0:
      if (funct7 == 0x00) { // ADD
        std::cout << "ADD: x" << rd << " = x" << rs1 << " + x" << rs2
                  << std::endl;
        regs[rd] = regs[rs1] + regs[rs2];
      } else if (funct7 == 0x20) { // SUB
        std::cout << "SUB: x" << rd << " = x" << rs1 << " - x" << rs2
                  << std::endl;
        regs[rd] = regs[rs1] - regs[rs2];
      }
      break;

    case 0x1: // SLL (Shift Left Logical)
      // The shift amount is the lower 5 bits of rs2
      regs[rd] = regs[rs1] << (regs[rs2] & 0x1F);
      break;

    case 0x4: // XOR
      regs[rd] = regs[rs1] ^ regs[rs2];
      break;

    case 0x5:               // SRL or SRA
      if (funct7 == 0x00) { // SRL
        regs[rd] = regs[rs1] >> (regs[rs2] & 0x1F);
      } else if (funct7 == 0x20) { // SRA
        // Cast to signed int to force arithmetic shift (preserves sign bit)
        regs[rd] = (int32_t)regs[rs1] >> (regs[rs2] & 0x1F);
      }
      break;

    case 0x6: // OR
      regs[rd] = regs[rs1] | regs[rs2];
      break;

    case 0x7: // AND
      regs[rd] = regs[rs1] & regs[rs2];
      break;

    // TODO: SLT (Set Less Than) and SLTU (Unsigned)
    default:
      std::cerr << "Unknown R-Type funct3: " << funct3 << std::endl;
      break;
    }

    break;
  }
  case 0x23: { // S-Type: STORE (sb, sh, sw)
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t rs2 = (instruction >> 20) & 0x1F;

    // S-Type Immediate is split: bits 25-31 (top 7) and bits 7-11 (bottom 5)
    int32_t imm = ((int32_t)(instruction & 0xFE000000) >> 20) |
                  ((instruction >> 7) & 0x0000001F);

    // Target Address = Base Register + Offset
    uint32_t addr = regs[rs1] + imm;

    switch (funct3) {
    case 0x0: // SB (Store Byte)
      // Only write the lowerst 8 bits of rs2
      mem.write(addr, regs[rs2] & 0xFF);
      std::cout << "SB: mem[" << std::hex << addr
                << "] = " << (regs[rs2] & 0xFF) << std::endl;
      break;

    case 0x1: // SH (Store Halfword - 16 bits)
      // Write low byte to addr, high byte to addr+1
      mem.write(addr, regs[rs2] & 0xFF);
      mem.write(addr + 1, (regs[rs2] >> 8) & 0xFF);
      std::cout << "SH: mem[" << std::hex << addr
                << "] = " << (regs[rs2] & 0xFFFF) << std::endl;
      break;

    case 0x2: // SW (Store Word - 32 bits)
      // Write 4 bytes (Little Endian)
      mem.write(addr, regs[rs2] & 0xFF);
      mem.write(addr + 1, (regs[rs2] >> 8) & 0xFF);
      mem.write(addr + 2, (regs[rs2] >> 16) & 0xFF);
      mem.write(addr + 3, (regs[rs2] >> 24) & 0xFF);
      std::cout << "SW: mem[" << std::hex << addr << "] = " << regs[rs2]
                << std::endl;
      break;

    default:
      std::cerr << "Unknown Store funct3: " << funct3 << std::endl;
      break;
    }
    break;
  }
  case 0x03: {
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (int32_t)(instruction & 0xFFF00000) >> 20;

    uint32_t addr = regs[rs1] + imm;

    switch (funct3) {
    case 0x0: { // LB (Load Byte)
      // Read byte, cast to int8_t to get sign, then cast to uint32_t
      int8_t val = (int8_t)mem.read(addr);
      regs[rd] = (uint32_t)val; // Sign-extends automatically
      std::cout << "LB: x" << rd << " = mem[" << std::hex << addr << "]"
                << std::endl;
      break;
    }
    case 0x1: { // LH (Load Halfword - Signed)
      // Read 2 bytes
      uint16_t val = mem.read(addr) | (mem.read(addr + 1) << 8);
      regs[rd] = (int16_t)val;
      std::cout << "LH: x" << rd << " = mem[" << std::hex << addr << "]"
                << std::endl;
      break;
    }
    case 0x2: // LW (Load Word)
      regs[rd] = mem.read(addr) | (mem.read(addr + 1) << 8) |
                 (mem.read(addr + 2) << 16) | (mem.read(addr + 3) << 24);
      std::cout << "LW: x" << rd << " = mem[" << std::hex << addr << "]"
                << std::endl;
      break;

      // TODO: LBU (0x4) and LHU (0x5) - Unsigned versions

    default:
      std::cerr << "Unknown Load funct3: " << funct3 << std::endl;
      break;
    }
    break;
  }

  case 0x63: { // B-Type: BRANCH
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    uint32_t rs2 = (instruction >> 20) & 0x1F;

    // B-Type Immediate Scramble
    // First extract pieces
    int32_t imm_12 = (instruction >> 31) & 0x1;    // Bit 12 (Sign)
    int32_t imm_10_5 = (instruction >> 25) & 0x3F; // Bits 10:5
    int32_t imm_4_1 = (instruction >> 8) & 0xF;    // Bits 4:1
    int32_t imm_11 = (instruction >> 7) & 0x1;     // Bit 11

    // Next, reassemble. We cast sign bit to signed 32-bit to force sign
    // extension
    int32_t imm = ((imm_12 ? 0xFFFFF800 : 0) | (imm_11 << 11) |
                   (imm_10_5 << 5) | (imm_4_1 << 1));

    // The current PC is already pointing to Next_Instr (PC+4)
    // Branch offset is relative to the Branch_Instr itself
    uint32_t current_inst_pc = pc - 4;

    bool take_branch = false;

    switch (funct3) {
    case 0x0: // BEQ
      if (regs[rs1] == regs[rs1])
        take_branch = true;
      break;
    case 0x1: // BNE
      if (regs[rs1] != regs[rs2])
        take_branch = true;
      break;
    case 0x4: // BLT (Branch Less Than - Signed)
      if ((int32_t)regs[rs1] < (int32_t)regs[rs2])
        take_branch = true;
      break;
    case 0x5: // BGE
      if ((int32_t)regs[rs1] >= (int32_t)regs[rs2])
        take_branch = true;
      break;
    case 0x6: // BLTU
      if (regs[rs1] < regs[rs2])
        take_branch = true;
      break;
    case 0x7: // BGEU
      if (regs[rs1] >= regs[rs2])
        take_branch = true;
      break;
    default:
      std::cerr << "Unknown Branch funct3: " << funct3 << std::endl;
      break;
    }

    if (take_branch) {
      pc = current_inst_pc + imm;
      std::cout << "  -> Branch TAKEN. New PC: " << std::hex << pc << std::endl;
    } else {
      std::cout << "  -> Branch NOT taken." << std::endl;
    }
    break;
  }
  case 0x6F: { // J-Type: JAL (Jump and Link)
    uint32_t rd = (instruction >> 7) & 0x1F;

    int32_t imm_20 = (instruction >> 31) & 0x1;
    int32_t imm_10_1 = (instruction >> 21) & 0x3FF;
    int32_t imm_11 = (instruction >> 20) & 0x1;
    int32_t imm_19_12 = (instruction >> 12) & 0xFF;

    int32_t imm = (imm_20 ? 0xFFF00000 : 0) | (imm_19_12 << 12) |
                  (imm_11 << 11) | (imm_10_1 << 1);

    // This saves the return address (PC + 4) into the destination register
    regs[rd] = pc;

    // This updates PC to the target
    pc = (pc - 4) + imm;

    // std::cout << "JAL: Saved RA to x" << rd << ", Jumping to " << std::hex <<
    // pc
    //         << std::endl;
    break;
  }
  case 0x67: { // I-Type: JALR (Jump and Link Register)
    uint32_t rd = (instruction >> 7) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t rs1 = (instruction >> 15) & 0x1F;
    int32_t imm = (int32_t)(instruction & 0xFFF00000) >> 20;

    // Calculate target address (rs1 + imm)
    // Risc-v requires setting the least-significant bit to 0
    uint32_t target_pc = (regs[rs1] + imm) & ~1;

    regs[rd] = pc;

    // JUMP!
    pc = target_pc;

    std::cout << "JALR: Jump to " << std::hex << pc << std::endl;
    break;
  }
  case 0x37: { // U-Type: LUI (Load Upper Immediate)
    uint32_t rd = (instruction >> 7) & 0x1F;
    // Extract the top 20 bits and put them in the top of the register
    int32_t imm = instruction & 0xFFFFF000;
    regs[rd] = imm;
    std::cout << "LUI: x" << rd << " = " << std::hex << imm << std::endl;
    break;
  }
  case 0x17: { // U-Type: AUIPC (Add Upper Immediate to PC)
    uint32_t rd = (instruction >> 7) & 0x1F;
    int32_t imm = instruction & 0xFFFFF000;
    regs[rd] = (pc - 4) + imm;
    std::cout << "AUIPC: x" << rd << " = PC + " << std::hex << imm << std::endl;
    break;
  }
  }
}
