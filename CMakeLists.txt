cmake_minimum_required(VERSION 3.15...4.1)

project(riscv-emulator VERSION 1.0
                       DESCRIPTION "A functional cycle-accurate emulator for the RISC-V 32-bit integer instruction set"
                       LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#if(MSVC)
  #add_compile_options(/W4 /WX) # Windows flags
  #else()
  #add_compile_options(-Wall -Wextra -Werror)
  #endif()

add_executable(riscv src/cpu.cpp src/main.cpp src/memory.cpp)

target_include_directories(riscv PRIVATE src)



