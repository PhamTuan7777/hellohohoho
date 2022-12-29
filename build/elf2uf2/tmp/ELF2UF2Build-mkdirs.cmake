# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/vscodecc++/Pico/picoproject/pico-sdk/tools/elf2uf2"
  "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2"
  "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2"
  "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2/tmp"
  "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2/src/ELF2UF2Build-stamp"
  "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2/src"
  "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/vscodecc++/Pico/picoproject/TFT_TEST/build/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
