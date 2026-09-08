#include "hd44780_pico/lcd_parallel.h"
#include "hd44780_pico/lcd.h"

#include <cstdio>
#include <cstdint>

#include "hardware/gpio.h"

using namespace hd44780pico;

LcdDisplayParallelGpio::LcdDisplayParallelGpio(Mapping mapping)
    : LcdDisplay(mapping) {}

void LcdDisplayParallelGpio::send_buffer() const {
    const auto buf = buffer();
    const auto pin_map = mapping();
    const int end = is_8_bit_enabled() ? NUM_PINS : D3;

    for (int i{0}; i < end; ++i) {
        const auto pin{pin_map[i]};
        if (pin == Mapping::unused_pin) {
            continue;
        }
        gpio_put(pin, (buf >> (11-i)) & 0x1);
    }
}

void LcdDisplayParallelGpio::init_io() const {
    const auto pin_map = mapping();
    const int end = is_8_bit_enabled() ? NUM_PINS : D3;

    for (int i{0}; i < end; ++i) {
        const auto pin{pin_map[i]};
        if (pin == Mapping::unused_pin) {
            continue;
        }
        gpio_init(pin);
        gpio_put(pin, false);
        gpio_set_dir(pin, GPIO_OUT);
    }
}
