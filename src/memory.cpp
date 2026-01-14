#include "memory.h"
#include <fstream>
#include <iostream>
#include <type_traits>

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

bool Memory::load_binary(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    return false;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (size > data.size()) {
    std::cerr << "Error: File size (" << size << ") exceeds memory size ("
              << data.size() << ")" << std::endl;
    return false;
  }

  // Read file into the data vector
  if (!file.read(reinterpret_cast<char *>(data.data()), size)) {
    std::cerr << "Error: Failed to read file data." << std::endl;
    return false;
  }

  std::cout << "Loaded " << size << " bytes from " << filename << std::endl;
  return true;
}
