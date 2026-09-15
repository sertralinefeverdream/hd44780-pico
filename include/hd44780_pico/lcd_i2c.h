#ifndef HD44780_LCD_I2C_H
#define HD447800_LCD_I2C_H

#include "hd44780_pico/lcd.h"
#include "hardware/i2c.h"

namespace hd44780pico {
    class LcdDisplayI2c : public LcdDisplay {
    public: 
        explicit LcdDisplayI2c(Mapping mapping, i2c_inst_t* i2c, std::uint8_t addr);
    protected:
        void send_buffer() const override;
        void init(BitMode bit_mode=BitMode::FOUR_BIT, LineMode line_mode=LineMode::TWO_LINES_5_8) override;
    private:
        i2c_inst_t* i2c_;
        std::uint8_t addr_;
    };
}

#endif