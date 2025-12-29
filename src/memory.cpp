#include "memory.h"

Memory::Memory(size_t size) : data(size) {}

uint8_t Memory::read(uint32_t address) { return 0; }

void Memory::write(uint32_t address, uint8_t value) {}

size_t Memory::get_size() const { return data.size(); }
