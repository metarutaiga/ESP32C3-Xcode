include($ENV{IDF_PATH}/tools/cmake/utilities.cmake)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_ASM_COMPILER clang-15)

set(CMAKE_AR riscv32-esp-elf-ar)
set(CMAKE_RANLIB riscv32-esp-elf-gcc-ranlib)
set(CMAKE_OBJDUMP riscv32-esp-elf-objdump)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=${IDF_PATH}/../build/esp-clang/riscv32-esp-elf/bin/ld -L${IDF_PATH}/../build/esp-clang/riscv32-esp-elf/lib/rv32imc/ilp32")

# -freestanding is a hack to force Clang to use its own stdatomic.h,
# without falling back to the (incompatible) GCC stdatomic.h
# https://github.com/espressif/llvm-project/blob/d9341b81/clang/lib/Headers/stdatomic.h#L13-L18
remove_duplicated_flags("-target riscv32-esp-elf -march=rv32imc -ffreestanding ${CMAKE_C_FLAGS}"
                        UNIQ_CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${UNIQ_CMAKE_C_FLAGS} -I${IDF_PATH}/../build/esp-clang/riscv32-esp-elf/include"
    CACHE STRING "C Compiler Base Flags"
    FORCE)

remove_duplicated_flags("-target riscv32-esp-elf -march=rv32imc -ffreestanding ${CMAKE_CXX_FLAGS}"
                        UNIQ_CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${UNIQ_CMAKE_CXX_FLAGS} -I${IDF_PATH}/../build/esp-clang/riscv32-esp-elf/include/c++/v1 -D_LIBCPP_HAS_NO_THREADS -I${IDF_PATH}/../build/esp-clang/riscv32-esp-elf/include"
    CACHE STRING "C++ Compiler Base Flags"
    FORCE)

remove_duplicated_flags("-target riscv32-esp-elf -march=rv32imc ${CMAKE_ASM_FLAGS}"
                        UNIQ_CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${UNIQ_CMAKE_ASM_FLAGS}"
    CACHE STRING "Assembler Base Flags"
    FORCE)
