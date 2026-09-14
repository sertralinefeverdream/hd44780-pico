#ifndef HD44780_LCD_I2C_H
#define HD447800_LCD_I2C_H

#include "hd44780_pico/lcd.h"
#include "hardware/i2c.h"

namespace hd44780pico {
    class LcdDisplayI2c : public LcdDisplay {
    public: 
        explicit LcdDisplayI2c(Mapping mapping);
    protected:
        void send_buffer() const override;
    };
}

#endif