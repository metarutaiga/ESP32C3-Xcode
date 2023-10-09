#!/bin/bash

if [ ! -f "riscv32-esp-elf-12.2.0_20230208-aarch64-apple-darwin.tar.xz" ]; then 
  wget -N https://github.com/espressif/crosstool-NG/releases/download/esp-12.2.0_20230208/riscv32-esp-elf-12.2.0_20230208-aarch64-apple-darwin.tar.xz
fi

if [ ! -f "llvm-esp-16.0.0-20230516-macos-arm64.tar.xz" ]; then 
  wget -N https://github.com/espressif/llvm-project/releases/download/esp-16.0.0-20230516/llvm-esp-16.0.0-20230516-macos-arm64.tar.xz
fi

if [ ! -d "../riscv32-esp-elf" ]; then
  cd ..
  tar xvf build/riscv32-esp-elf-12.2.0_20230208-aarch64-apple-darwin.tar.xz
  xattr -r -d com.apple.quarantine riscv32-esp-elf
  cd build
fi

if [ ! -d "../esp-clang" ]; then
  cd ..
  tar xvf build/llvm-esp-16.0.0-20230516-macos-arm64.tar.xz
  xattr -r -d com.apple.quarantine esp-clang
  rm esp-clang/bin/clang
  rm esp-clang/bin/clang++
  cp build/esp-clang-clang esp-clang/bin/clang
  cp build/esp-clang-clang++ esp-clang/bin/clang++
  ln -s /opt/homebrew/opt/llvm/include/c++/v1 esp-clang/riscv32-esp-elf/include/c++/v1
  echo !\<arch\> >esp-clang/riscv32-esp-elf/lib/rv32imc/ilp32/libc++.a
  echo !\<arch\> >esp-clang/riscv32-esp-elf/lib/rv32imc/ilp32/libc++abi.a
  echo !\<arch\> >esp-clang/riscv32-esp-elf/lib/rv32imc/ilp32/libclang_rt.builtins-riscv32.a
  echo !\<arch\> >esp-clang/riscv32-esp-elf/lib/rv32imc/ilp32/libunwind.a
  cd build
fi

if [ ! -f "patchsdk.ok" ]; then
  echo ok > patchsdk.ok
  cd ../esp-idf
  patch < ../build/patchsdk
  cp ../build/toolchain-clang-esp32c3.cmake tools/cmake
fi

if [ ! -d "../esp-idf/components/esp_hap_apple_profiles" ]; then ln -s ../../esp-homekit-sdk/components/homekit/esp_hap_apple_profiles ../esp-idf/components/esp_hap_apple_profiles; fi
if [ ! -d "../esp-idf/components/esp_hap_core" ]; then ln -s ../../esp-homekit-sdk/components/homekit/esp_hap_core ../esp-idf/components/esp_hap_core; fi
if [ ! -d "../esp-idf/components/esp_hap_extras" ]; then ln -s ../../esp-homekit-sdk/components/homekit/esp_hap_extras ../esp-idf/components/esp_hap_extras; fi
if [ ! -d "../esp-idf/components/esp_hap_platform" ]; then ln -s ../../esp-homekit-sdk/components/homekit/esp_hap_platform ../esp-idf/components/esp_hap_platform; fi
if [ ! -d "../esp-idf/components/hkdf-sha" ]; then ln -s ../../esp-homekit-sdk/components/homekit/hkdf-sha ../esp-idf/components/hkdf-sha; fi
if [ ! -d "../esp-idf/components/json_generator" ]; then ln -s ../../esp-homekit-sdk/components/homekit/json_generator ../esp-idf/components/json_generator; fi
if [ ! -d "../esp-idf/components/json_parser" ]; then ln -s ../../esp-homekit-sdk/components/homekit/json_parser ../esp-idf/components/json_parser; fi
if [ ! -d "../esp-idf/components/mu_srp" ]; then ln -s ../../esp-homekit-sdk/components/homekit/mu_srp ../esp-idf/components/mu_srp; fi
