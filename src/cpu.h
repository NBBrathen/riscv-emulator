#pragma once
#include "memory.h"
#include <array>
#include <cstdint>

class CPU {
public:
  // CPU takes a reference to Memory on startup
  explicit CPU(Memory &mem);

  void step();

  void dump_registers() const;

  uint32_t get_pc() const { return pc; }

private:
  uint32_t fetch();
  void execute(uint32_t instruction);
  std::array<uint32_t, 32> regs;
  uint32_t pc;
  Memory &mem;
};
