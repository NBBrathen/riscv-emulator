#include "memory.h"

Memory::Memory(size_t size) : data(size) {}

uint8_t Memory::read(uint32_t address) {
  if (address >= data.size()) {
    return 0;
  }
  return data[address];
}

void Memory::write(uint32_t address, uint8_t value) {
  if (address >= data.size()) {
    return;
  }
  data[address] = value;
}

size_t Memory::get_size() const { return data.size(); }
