#include "hd44780_pico/lcd_i2c.h"
#include "hd44780_pico/lcd.h"

#include "hardware/i2c.h"

#include <cstdint>

using namespace hd44780pico;

LcdDisplayI2c::LcdDisplayI2c(Mapping mapping)
    : LcdDisplay(mapping) {}

