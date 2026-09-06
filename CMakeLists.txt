cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hd44780_pico)

add_library(hd44780_pico)

target_sources(hd44780_pico
    PRIVATE
        src/lcd.cpp
        src/lcd_parallel.cpp
        
    PUBLIC
        FILE_SET hd44780_headers
        TYPE HEADERS
        BASE_DIRS
            include
        FILES
            include/hd44780_pico/lcd.h
            include/hd44780_pico/lcd_parallel.h
)

target_include_directories(hd44780_pico
    PUBLIC
        include
)

target_link_libraries(hd44780_pico
    PRIVATE
        pico_stdlib 
        hardware_gpio
)