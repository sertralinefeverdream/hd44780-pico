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

    // Delays
    constexpr int delay_ms_pulse_enable{2};
}
//Base class public impls

LcdDisplay::LcdDisplay()
    : entry_mode_set_{bm_ems | bm_ems_increment}
    , display_control_{bm_dc | bm_dc_display_on | bm_dc_cursor_on | bm_dc_blinking_on}
    , cursor_display_shift_{bm_cds | bm_cds_dir_right}
    , function_set_{bm_fs} {}
    
void LcdDisplay::init(bool enable_two_lines, bool enable_8_bit, bool enable_5_by_10) {
    if (enable_two_lines) {
        function_set_ |= bm_fs_two_lines;
    }
        
    if (enable_8_bit) {
        function_set_ |= bm_fs_8_bit_mode;
    }
        
    if (enable_5_by_10) {
        function_set_ |= bm_fs_5_by_10_font;

    }    
    init_io();    
    if (display_control_ & bm_fs_8_bit_mode)  {
        init_sequence_8_bit();
    } else {
        init_sequence_4_bit();
    }
}

// Base class private impls
void LcdDisplay::execute_instruction(std::uint16_t instr) { 
    buffer_ &= bm_clear_buffer_data;
    if (display_control_ & bm_fs_8_bit_mode) { 
        const auto rs_rw {
            static_cast<std::uint16_t>(instr & bm_rs_rw)
        };
        
        const auto upper_nibble {
            static_cast<std::uint16_t>((instr & bm_upper_nibble) | rs_rw)
        };
        
        const auto lower_nibble {
            static_cast<std::uint16_t>(((instr & bm_lower_nibble) << 4) | rs_rw)
        };
        
        buffer_ |= upper_nibble;
        pulse_enable(); 
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
    buffer_ &= ~(bm_enable_e);
    send_buffer();
}
