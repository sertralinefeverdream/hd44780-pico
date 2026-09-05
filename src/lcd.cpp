#include "../include/hd44780_lib/lcd.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

using namespace hd44780pico;

namespace {
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
    
    constexpr uint16_t bm_set_ddram{0b00'1000'0000};
    constexpr int delay_ms_pulse_enable{2};
}
// Base Class
LcdDisplay::LcdDisplay(OperationMode op_mode, LineMode line_mode);

// private impls
