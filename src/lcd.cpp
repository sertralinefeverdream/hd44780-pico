#include "hd44780_pico/lcd.h"
//#include "../include/hd44780_pico/lcd.h"

#include <algorithm>
#include <functional>
#include <string_view>
#include <cstddef>
#include <cassert>
#include <cstdio>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

using namespace hd44780pico;

namespace {
    // Bitmasks
    constexpr std::uint16_t bm_clear_buffer_data_pins{0b1100'0000'0000};
    constexpr std::uint16_t bm_upper_nibble{0b00'1111'0000}; //
    constexpr std::uint16_t bm_lower_nibble{0b00'0000'1111}; //
    constexpr std::uint16_t bm_rs_rw{0b11'0000'0000}; 
    
    constexpr std::uint16_t bm_clear_display{0b1};
    constexpr std::uint16_t bm_return_home{0b10};

    constexpr std::uint16_t bm_ems{0b00'0000'0100};
    constexpr std::uint16_t bm_ems_increment{0b10};
    constexpr std::uint16_t bm_ems_shift_display{0b1};
    
    constexpr std::uint16_t bm_dc{0b00'0000'1000};
    constexpr std::uint16_t bm_dc_display_on{0b100};
    constexpr std::uint16_t bm_dc_cursor_on{0b10};
    constexpr std::uint16_t bm_dc_blinking_on{0b1};
    
    constexpr std::uint16_t bm_cds{0b00'0001'0000};
    constexpr std::uint16_t bm_cds_shift_screen{0b1000};
    constexpr std::uint16_t bm_cds_dir_right{0b100};
    
    constexpr std::uint16_t bm_fs{0b00'0010'0000};
    constexpr std::uint16_t bm_fs_8_bit_mode{0b1'0000};
    constexpr std::uint16_t bm_fs_two_lines{0b1000};
    constexpr std::uint16_t bm_fs_5_by_10_font{0b100};
    
    constexpr std::uint16_t bm_write_ddram_data{0b10'0000'0000};
    constexpr std::uint16_t bm_set_ddram_addr{0b00'1000'0000};
    constexpr std::uint16_t bm_ddram_addr{0b00'0111'1111};
    constexpr std::uint16_t bm_ddram_data{0b00'1111'1111};
    
    constexpr std::uint16_t bm_backlight{0b1000'0000'0000};
    constexpr std::uint16_t bm_enable_e{0b0100'0000'0000};
    
    constexpr std::uint16_t bm_init{0b00'0011'0000};
    constexpr std::uint16_t bm_blank{0};

    // Delays
    constexpr int delay_ms_clear_display{2};
    constexpr int delay_us_pulse_enable{2};
    constexpr int delay_ms_return_home{2};
    constexpr int delay_us_ems{38};
    constexpr int delay_us_dc{38};
    constexpr int delay_us_cds{38};
    constexpr int delay_us_fs{38};
    constexpr int delay_us_set_ddram{38};
    constexpr int delay_us_write_ddram{38};
}

// Lcd Base class public impls
// 

std::uint8_t Mapping::operator[](std::size_t pin) const {
    assert(pin < LcdPin::NUM_PINS);
    switch (pin) {
        case BACKLIGHT: 
            return backlight;
        case E:
            return e;
        case RS:
            return rs;
        case RW:
            return rw;
        case D7:
            return d7;
        case D6:
            return d6;
        case D5:
            return d5;
        case D4:
            return d4;
        case D3:
            return d3;
        case D2:
            return d2;
        case D1:
            return d1;
        case D0:
            return d0;
        default:
            return unused_pin;
    }
}

LcdDisplay::LcdDisplay(Mapping mapping)
    : mapping_{mapping}
    , entry_mode_set_cmd_{bm_ems | bm_ems_increment}
    , display_control_cmd_{bm_dc | bm_dc_display_on}
    , cursor_display_shift_cmd_{bm_cds | bm_cds_dir_right}
    , function_set_cmd_{bm_fs} {}
    
void LcdDisplay::init(BitMode bit_mode, LineMode line_mode) {
    if (line_mode == LineMode::ONE_LINE_5_10) {
        function_set_cmd_ |= bm_fs_5_by_10_font;
    } else if (line_mode == LineMode::TWO_LINES_5_8) {
        function_set_cmd_ |= bm_fs_two_lines;
    }
    
    if (bit_mode == BitMode::EIGHT_BIT) {
        function_set_cmd_ |= bm_fs_8_bit_mode;
    }
    
    init_io();    
    init_sequence();
}

void LcdDisplay::clear_display() {
    execute_cmd(bm_clear_display);
    sleep_ms(delay_ms_clear_display);
}

void LcdDisplay::return_home() {
    execute_cmd(bm_return_home);
    sleep_ms(delay_ms_return_home);
}

void LcdDisplay::putc(char c) {
    write_ddram_data(static_cast<std::uint16_t>(c));
}

void LcdDisplay::puts(std::string_view s) {
    for (auto c : s) {
        putc(c);
    }
}

void LcdDisplay::set_display_enabled(bool enabled) { 
    if (enabled) {
        display_control_cmd_ |= bm_dc_display_on;
    } else {
        display_control_cmd_ &= ~bm_dc_display_on;
    }
    
    display_control();
}

void LcdDisplay::set_backlight_enabled(bool enabled) {
    if (enabled) {
        buffer_ |= bm_backlight;
    } else {
        buffer_ &= ~bm_backlight;
    }
    
    send_buffer();
}

void LcdDisplay::set_blink_enabled(bool enabled) {
    if (enabled) {
        display_control_cmd_ |= bm_dc_blinking_on;
    } else {
        display_control_cmd_ &= ~bm_dc_blinking_on;
    }
    
    display_control();
}

void LcdDisplay::set_cursor_enabled(bool enabled) {
    if (enabled) {
        display_control_cmd_ |= bm_dc_cursor_on;
    } else {
        display_control_cmd_ &= ~bm_dc_cursor_on; 
    }
    
    display_control();
}

void LcdDisplay::set_write_direction(Direction dir) {
    if (dir == Direction::LEFT) {
        entry_mode_set_cmd_ &= ~bm_ems_increment;
    } else {
        entry_mode_set_cmd_ |= bm_ems_increment;
    }
    
    entry_mode_set();
}

void LcdDisplay::set_display_shift_enabled(bool enabled) {
    if (enabled) {
        entry_mode_set_cmd_ |= bm_ems_shift_display;
    } else {
        entry_mode_set_cmd_ &= ~bm_ems_shift_display;
    }
    
    entry_mode_set();
}

void LcdDisplay::move_cursor(int n, Direction dir) {
    if (dir == Direction::LEFT) {
        cursor_display_shift_cmd_ &= ~bm_cds_dir_right;
    } else {
        cursor_display_shift_cmd_ |= bm_cds_dir_right;
    }
    
    cursor_display_shift_cmd_ &= ~bm_cds_shift_screen;
    
    for (auto i{0}; i < n; ++i) {
        cursor_display_shift();        
    }
}

void LcdDisplay::move_display(int n, Direction dir) {
    if (dir == Direction::LEFT) {
        cursor_display_shift_cmd_ &= ~bm_cds_dir_right;
    } else {
        cursor_display_shift_cmd_ |= bm_cds_dir_right;
    }
    
    cursor_display_shift_cmd_ |= bm_cds_shift_screen;
    
    for (auto i{0}; i < n; ++i) {
        cursor_display_shift();
    }
}

void LcdDisplay::cursor_goto(std::uint16_t col, std::uint16_t row=0) {
    const std::uint16_t max_col = is_two_lines_enabled() ? 39 : 79;
    const std::uint16_t max_row = is_two_lines_enabled() ? 1 : 0;

    col = std::clamp(col, (uint16_t)0, max_col);
    row = std::clamp(row, (uint16_t)0, max_row);
    set_ddram_addr(row > 0 ? (col | 0x40) : col);
}

// Protected impls
Mapping LcdDisplay::mapping() const {
    return mapping_;
}


std::uint16_t LcdDisplay::buffer() const {
    return buffer_;
}

bool LcdDisplay::is_8_bit_enabled() const {
    return function_set_cmd_ & bm_fs_8_bit_mode;
}

bool LcdDisplay::is_two_lines_enabled() const {
    return function_set_cmd_ & bm_fs_two_lines;
}

// Private impls

void LcdDisplay::clear_buffer_data_pins() {
    buffer_ &= bm_clear_buffer_data_pins;
}

void LcdDisplay::entry_mode_set() {
    execute_cmd(entry_mode_set_cmd_);
    sleep_us(delay_us_ems);
}

void LcdDisplay::display_control() {
    execute_cmd(display_control_cmd_);
    sleep_us(delay_us_dc);
}

void LcdDisplay::cursor_display_shift() {
    execute_cmd(cursor_display_shift_cmd_) ;
    sleep_us(delay_us_cds);
}

void LcdDisplay::function_set() {
    printf("FUNCTION SET = 0x%02X\n", function_set_cmd_);
    execute_cmd(function_set_cmd_);
    sleep_us(delay_us_fs);
}

void LcdDisplay::set_ddram_addr(std::uint16_t addr) {  
    addr &= bm_ddram_addr;
    addr |= bm_set_ddram_addr;
    execute_cmd(addr);
    sleep_us(delay_us_set_ddram);
}

void LcdDisplay::write_ddram_data(std::uint16_t data) {
    data &= bm_ddram_data;
    data |= bm_write_ddram_data;
    execute_cmd(data);
    sleep_us(delay_us_write_ddram);
}

void LcdDisplay::execute_cmd(std::uint16_t instr) { 
    //printf("instr = 0x%03X\n", instr);
    if (is_8_bit_enabled()) { 
        clear_buffer_data_pins();
        buffer_ |= instr; 
        pulse_enable();
    } else {
        const std::uint16_t rs_rw = instr & bm_rs_rw;
        const std::uint16_t upper_nibble = (instr & bm_upper_nibble) | rs_rw;
        const std::uint16_t lower_nibble = ((instr & bm_lower_nibble) << 4) | rs_rw;

        clear_buffer_data_pins();
        buffer_ |= upper_nibble;
        pulse_enable();
        clear_buffer_data_pins();
        buffer_ |= lower_nibble;
        pulse_enable();
    }
}

                             //
void LcdDisplay::pulse_enable() {
    buffer_ |= bm_enable_e;
    send_buffer();
    sleep_us(delay_us_pulse_enable);
    buffer_ &= ~(bm_enable_e);
    send_buffer();
    sleep_us(delay_us_pulse_enable);
}

void LcdDisplay::init_sequence() { 
    clear_buffer_data_pins();
    sleep_ms(41);
    buffer_ |= bm_init;
    pulse_enable();
    sleep_ms(5);
    pulse_enable();
    sleep_us(101);
    pulse_enable();
    
    if (!(is_8_bit_enabled())) {
        clear_buffer_data_pins();
        buffer_ |= 0x20; 
        pulse_enable();
    }
    
    function_set();
    set_display_enabled(false);
    clear_display();
    entry_mode_set();
    set_display_enabled(true);
}


