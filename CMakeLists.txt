cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hd44780_pico)

add_library(hd44780_pico)

target_sources(hd44780_pico
    PRIVATE
        src/lcd.cpp
        src/lcd_gpio.cpp
        src/lcd_i2c.cpp
        
    PUBLIC
        FILE_SET hd44780_headers
        TYPE HEADERS
        BASE_DIRS
            include
        FILES
            include/hd44780_pico/lcd.h
            include/hd44780_pico/lcd_gpio.h
            include/hd44780_pico/lcd_i2c.h
)

target_include_directories(hd44780_pico
    PUBLIC
        include
)

target_link_libraries(hd44780_pico
    PRIVATE
        pico_stdlib 
        hardware_gpio
        hardware_i2c
)