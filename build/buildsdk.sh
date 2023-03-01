#!/bin/bash

export BUILD_PATH=$PWD
export IDF_PATH=$PWD/../esp-idf
export PATH=$PWD:$PWD/esp-clang/bin:$PATH

if [ ! -f "hello_world/README.md" ]; then
  cp -r $IDF_PATH/examples/get-started/hello_world .
fi

cd hello_world
if [ ! -d "build" ]; then
  idf.py -D IDF_TOOLCHAIN=clang set-target esp32-c3
fi
idf.py -D IDF_TOOLCHAIN=clang menuconfig
if [ ! -d "../esp-clang/riscv32-esp-elf/include/c++" ]; then
  mv ../esp-clang/riscv32-esp-elf/include/c++- ../esp-clang/riscv32-esp-elf/include/c++
fi
idf.py -D IDF_TOOLCHAIN=clang build
if [ ! -d "../esp-clang/riscv32-esp-elf/include/c++-" ]; then
  mv ../esp-clang/riscv32-esp-elf/include/c++ ../esp-clang/riscv32-esp-elf/include/c++-
fi
cd ..
