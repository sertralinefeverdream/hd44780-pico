#ifndef HD44780_LCD_PARALLEL_H
#define HD44780_LCD_PARALLEL_H

#include "lcd.h"
// #include "hd44780_pico/lcd.h"

namespace hd44780pico {
    class LcdDisplayParallelGpio : LcdDisplay {
    public:        
        explicit LcdDisplayParallelGpio(Mapping mapping_);
    protected:
        void send_buffer() const override;
        void init_io() const override;
    };
}

#endif