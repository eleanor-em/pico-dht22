cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(dht22-pico)

pico_sdk_init()

add_executable(dht22-pico
        src/dht22-pico.c
        src/main.c
        )

pico_enable_stdio_usb(dht22-pico 1)

target_link_libraries(dht22-pico pico_stdlib)

pico_add_extra_outputs(dht22-pico)
