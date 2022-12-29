cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico-projects)

pico_sdk_init()

add_executable(main main.c)

add_subdirectory(TFT_LIB)
target_link_libraries(main pico_stdlib hardware_spi ILI9431)

pico_add_extra_outputs(main)