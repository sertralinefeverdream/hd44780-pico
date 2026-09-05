#include "../include/hd44780_lib/lcd.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

using namespace hd44780pico;

namespace {
    // Bitmasks
    constexpr uint16_t bm_clear_buffer_data{0b1100'0000'0000};
    constexpr uint16_t bm_upper_nibble{0x0F0};
    constexpr uint16_t bm_lower_nibble{0x00F};
    constexpr uint16_t bm_rs_rw{0x300};
    
    constexpr uint16_t bm_clear_display{0b1};
    constexpr uint16_t bm_return_home{0b10};

    constexpr uint16_t bm_ems{0b00'0000'0100};
    constexpr uint16_t bm_ems_increment{0b10};
    constexpr uint16_t bm_ems_shift_display{0b1};
    
    constexpr uint16_t bm_dc{0b00'0000'1000};
    constexpr uint16_t bm_dc_display_on{0b100};
    constexpr uint16_t bm_dc_cursor_on{0b10};
    constexpr uint16_t bm_dc_blinking_on{0b1};
    
    constexpr uint16_t bm_cds{0b00'0001'0000};
    constexpr uint16_t bm_cds_shift_screen{0b1000};
    constexpr uint16_t bm_cds_dir_right{0b100};
    
    constexpr uint16_t bm_fs{0b00'0010'0000};
    constexpr uint16_t bm_fs_8_bit_mode{0b10000};
    constexpr uint16_t bm_fs_two_lines{0b1000};
    constexpr uint16_t bm_fs_5_by_10_font{0b100};
    
    constexpr uint16_t bm_backlight{0b1000'0000'0000};
    constexpr uint16_t bm_enable_e{0b0100'0000'0000};
    constexpr uint16_t bm_set_ddram{0b00'1000'0000};
    
    constexpr uint16_t bm_init{0b00'0011'0000};
    constexpr uint16_t bm_blank{0};

    // Delays
    constexpr int delay_us_clear_display{2};
    constexpr int delay_us_pulse_enable{2};
    constexpr int delay_ms_return_home{2};
    constexpr int delay_us_ems{38};
    constexpr int delay_us_dc{38};
    constexpr int delay_us_cds{38};
    constexpr int delay_us_fs{38};
    constexpr int delay_us_set_ddram{38};
}
//Base class public impls

LcdDisplay::LcdDisplay()
    : entry_mode_set_{bm_ems | bm_ems_increment}
    , display_control_{bm_dc | bm_dc_display_on | bm_dc_cursor_on | bm_dc_blinking_on}
    , cursor_display_shift_{bm_cds | bm_cds_dir_right}
    , function_set_{bm_fs} {}
    
void LcdDisplay::init(bool enable_8_bit, LineMode mode) {
    if (mode == LineMode::ONE_LINE_5_10) {
        function_set_ |= bm_fs_5_by_10_font;
    } else if (mode == LineMode::TWO_LINES_5_8) {
        function_set_ |= bm_fs_two_lines;
    }
    
    if (enable_8_bit) {
        function_set_ |= bm_fs_8_bit_mode;
    }
    
    init_io();    
    init_sequence();
}

void LcdDisplay::clear_display() {
    execute_instruction(bm_clear_display);
    sleep_us(delay_us_clear_display);
}

void LcdDisplay::return_home() {
    execute_instruction(bm_return_home);
    sleep_ms(delay_ms_return_home);
}

void LcdDisplay::set_display_enabled(bool enabled) { 
    if (enabled) {
        display_control_ |= bm_dc_display_on;
    } else {
        display_control_ &= ~bm_dc_display_on;
    }
    
    execute_display_control();
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
        display_control_ |= bm_dc_blinking_on;
    } else {
        display_control_ &= ~bm_dc_blinking_on;
    }
    
    execute_display_control();
}

void LcdDisplay::set_cursor_enabled(bool enabled) {
    if (enabled) {
        display_control_ |= bm_dc_cursor_on;
    } else {
        display_control_ &= ~bm_dc_cursor_on; 
    }
    
    execute_display_control();
}

void LcdDisplay::set_write_direction(Direction dir) {
    if (dir == Direction::LEFT) {
        entry_mode_set_ &= ~bm_ems_increment;
    } else {
        entry_mode_set_ |= bm_ems_increment;
    }
    
    execute_entry_mode_set();
}

void LcdDisplay::set_display_shift_enabled(bool enabled) {
    if (enabled) {
        entry_mode_set_ |= bm_ems_shift_display;
    } else {
        entry_mode_set_ &= ~bm_ems_shift_display;
    }
    
    execute_entry_mode_set();
}

void LcdDisplay::move_cursor(int n, Direction dir) {
    if (dir == Direction::LEFT) {
        cursor_display_shift_ &= ~bm_cds_dir_right;
    } else {
        cursor_display_shift_ |= bm_cds_dir_right;
    }
    
    cursor_display_shift_ &= ~bm_cds_shift_screen;
    
    for (auto i{0}; i < n; ++i) {
        execute_cursor_display_shift();        
    }
}

void LcdDisplay::move_display(int n, Direction dir) {
    if (dir == Direction::LEFT) {
        cursor_display_shift_ &= ~bm_cds_dir_right;
    } else {
        cursor_display_shift_ |= bm_cds_dir_right;
    }
    
    cursor_display_shift_ |= bm_cds_shift_screen;
    
    for (auto i{0}; i < n; ++i) {
        execute_cursor_display_shift();
    }
}

// Base class private impls

void LcdDisplay::execute_entry_mode_set() {
    execute_instruction(entry_mode_set_);
    sleep_us(delay_us_ems);
}

void LcdDisplay::execute_display_control() {
    execute_instruction(display_control_);
    sleep_us(delay_us_dc);
}

void LcdDisplay::execute_cursor_display_shift() {
    execute_instruction(cursor_display_shift_) ;
    sleep_us(delay_us_cds);
}

void LcdDisplay::execute_function_set() {
    execute_instruction(function_set_);
    sleep_us(delay_us_fs);
}

void LcdDisplay::execute_instruction(std::uint16_t instr) { 
    buffer_ &= bm_clear_buffer_data;
    if (display_control_ & bm_fs_8_bit_mode) { 
        const auto rs_rw = static_cast<std::uint16_t>(instr & bm_rs_rw);
        const auto upper_nibble  = static_cast<std::uint16_t>((instr & bm_upper_nibble) | rs_rw);
        const auto lower_nibble = static_cast<std::uint16_t>(((instr & bm_lower_nibble) << 4) | rs_rw);
        
        
        buffer_ |= upper_nibble;
        pulse_enable();
        buffer_ &= bm_clear_buffer_data;
        buffer_ |= lower_nibble;
        pulse_enable();
    } else {
        buffer_ |= instr; 
        pulse_enable();
    }
}

void LcdDisplay::pulse_enable() {
    buffer_ |= bm_enable_e;
    send_buffer();
    sleep_us(delay_us_pulse_enable);
    buffer_ &= ~(bm_enable_e);
    sleep_us(delay_us_pulse_enable);
    send_buffer();
}

void LcdDisplay::init_sequence() { 
    buffer_ &= bm_clear_buffer_data; 
    sleep_ms(41);
    buffer_ |= bm_init;
    send_buffer();
    sleep_ms(5);
    send_buffer();
    sleep_us(101);
    send_buffer();
    
    if (!(function_set_ & bm_fs_8_bit_mode)) {
        buffer_ &= bm_clear_buffer_data; 
        buffer_ |= 0x20; 
        send_buffer();
    }
    
    execute_instruction(function_set_);
    sleep_us(delay_us_fs);
    set_display_enabled(false);
    clear_display();
    execute_entry_mode_set();
    set_display_enabled(true);
}

