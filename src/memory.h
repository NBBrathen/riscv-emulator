#pragma once
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>

class Memory {
public:
  // Initializer takes in size of memory (e.g., 1MB)
  Memory(size_t size);

  uint32_t read_word(uint32_t address);

  uint8_t read(uint32_t address);
  void write(uint32_t address, uint8_t value);

  bool load_binary(const std::string &filename);

  size_t get_size() const;

private:
  std::vector<uint8_t> data;
};
