#include "cpu.h"
#include <cstdint>

CPU::CPU(Memory &memory) : mem(memory) {}

void CPU::step() {}

void CPU::dump_registers() const {}
