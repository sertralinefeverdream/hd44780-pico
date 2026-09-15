#include "hd44780_pico/lcd_i2c.h"
#include "hd44780_pico/lcd.h"

#include "hardware/i2c.h"

#include <cstdint>

using namespace hd44780pico;

LcdDisplayI2c::LcdDisplayI2c(Mapping mapping, i2c_inst_t* i2c, std::uint8_t addr)
    : LcdDisplay(mapping)
    , i2c_{i2c} 
    , addr_{addr} {}
    
void LcdDisplayI2c::init(BitMode bit_mode, LineMode line_mode) {
    LcdDisplay::init(BitMode::FOUR_BIT, line_mode);
}
    
void LcdDisplayI2c::send_buffer() const {
    std::uint8_t i2c_buf{};
    const auto buf = buffer();
    const auto pin_map = mapping();
    constexpr int end{D3};
    
    for (auto i{0}; i < end; ++i) {
        const auto pin{pin_map[i]};
        if (pin == Mapping::unused_pin) {
            continue;
        }
        std::uint8_t bit = (buf >> (NUM_PINS - i - 1)) & 1;
        i2c_buf |= bit << pin_map[i];
    }
    
    i2c_write_blocking(i2c_, addr_, &i2c_buf, 1, false);
}

