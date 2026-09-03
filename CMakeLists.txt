cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 23)

project(hd44780_pico)

add_library(hd44780_pico)

target_sources()

target_link_libraries(hd44780_pico
    PRIVATE
        pico_stdlib 
        hardware_gpio
)