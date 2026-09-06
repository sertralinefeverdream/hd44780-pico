//#include "hd44780_pico/lcd_parallel.h"
//#include "hd44780_pico/lcd.h"

#include "../include/hd44780_pico/lcd_parallel.h"
#include "../include/hd44780_pico/lcd.h"


using namespace hd44780pico;

LcdDisplayParallelGpio::LcdDisplayParallelGpio(Mapping mapping)
    : LcdDisplay(mapping) {}

void LcdDisplayParallelGpio::send_buffer() const {
    auto buf = buffer();
    const int end = is_8_bit_enabled() ? NUM_PINS : D3;
    
    for (int i{0}; i < end; ++i) {
        const auto pin{mapping_[i]};
        if (pin == unused_pin) {
            continue;
        }
        gpio_put(pin, (buf >> (11-i)) & 0x1);
    }
}

void LcdDisplayParallelGpio::init_io() const {
    const int end = is_8_bit_enabled() ? NUM_PINS : D3;
    for (int i{0}; i < end; ++i) {
        const auto pin{mapping_[i]};
        if (pin == unused_pin) {
            continue;
        }
    }
}



