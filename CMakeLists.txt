cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(simple-pico-terminal C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

pico_sdk_init()

add_executable(simple-pico-terminal
    main.c
)

target_compile_options(simple-pico-terminal PUBLIC -Os)
target_link_libraries(simple-pico-terminal pico_stdlib)

pico_enable_stdio_usb(simple-pico-terminal 1)
pico_enable_stdio_uart(simple-pico-terminal 0)
pico_add_extra_outputs(simple-pico-terminal)






