#!/bin/bash

wget -N https://github.com/espressif/llvm-project/releases/download/esp-15.0.0-20221201/llvm-esp-15.0.0-20221201-macos.tar.xz

if [ ! -d "esp-clang" ]; then
  tar xvf llvm-esp-15.0.0-20221201-macos.tar.xz
fi
