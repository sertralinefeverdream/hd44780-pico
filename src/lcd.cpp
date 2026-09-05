#include "../include/hd44780_lib/lcd.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

using namespace hd44780pico;

namespace {
    constexpr int bm_enable{0b01'0000'0000};    
    constexpr int delay_ms_pulse_enable{2};
}
// Base Class
LcdDisplay::LcdDisplay(OperationMode op_mode, LineMode line_mode)
    : operation_mode_{op_mode}
    , line_mode_{line_mode} {} 

// private impls
void LcdDisplay::set_buffer_raw(uint16_t buf) { 
    buffer_ = buf;
}

void LcdDisplay::pulse_enable() {
    buffer_ |= bm_enable; 
    send_buffer();
    sleep_us(delay_ms_pulse_enable);
    buffer_ &= ~(bm_enable);
    send_buffer();
}