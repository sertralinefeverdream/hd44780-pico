#ifndef HD44780_LCD_PARALLEL_H
#define HD44780_LCD_PARALLEL_H

#include "hd44780_pico/lcd.h"

namespace hd44780pico {
    class LcdDisplayParallelGpio : public LcdDisplay {
    public:        
        explicit LcdDisplayParallelGpio(Mapping mapping);
    protected:
        void send_buffer() const override;
        void init_io() const override;
    };
}

#endif