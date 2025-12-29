#pragma once
#include <cstdint>
#include <vector>

class Memory {
public:
  // Initializer takes in size of memory (e.g., 1MB)
  Memory(size_t size);

  uint8_t read(uint32_t address);
  void write(uint32_t address, uint8_t value);

  size_t get_size() const;

private:
  std::vector<uint8_t> data;
};
