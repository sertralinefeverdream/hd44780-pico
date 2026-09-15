# hd44780-pico

- A C++ library implementing classes to manipulate any HD44780-compatible screen via the Pico C/C++ SDK. Assumes a 16x2 character LCD but theoretically should work with any dimensions. 
- Supports:
    - Parallel GPIO (4-bit mode and 8-bit mode)
    - I2C via PCF8574-like GPIO expanders (4-bit mode only)
    - Different line modes (5x8 one-line, 5x8 two-lines, 5x10 one-line)
    
## Example Usage (Parallel GPIO):
```cpp
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "hd44780_pico/lcd.h"
#include "hd44780_pico/lcd_i2c.h"

int main()
{
    stdio_init_all();
    
    using namespace hd44780pico;
    Mapping pin_map_4_bit{
            .backlight = Mapping::unused_pin,
            .e = 2,
            .rs = 0,
            .rw = Mapping::unused_pin,
            .d7 = 9,
            .d6 = 8,
            .d5 = 7,
            .d4 = 6,
    };
    
    Mapping pin_map_8_bit{
        .backlight = Mapping::unused_pin,
        .e = 2,
        .rs = 0,
        .rw = Mapping::unused_pin,
        .d7 = 9,
        .d6 = 8,
        .d5 = 7,
        .d4 = 6,
        .d3 = 5,
        .d2 = 4,
        .d1 = 3,
    }; // Mapping in this case maps each pin on the 16-header lcd to a respective GPIO pin.
    
    LcdDisplayParallelGpio lcd{pin_map_4_bit};
    lcd.init(BitMode::FOUR_BIT, LineMode::TWO_LINES_5_8);
    lcd.set_blink_enabled(true); 
    lcd.set_backlight_enabled(true);
    lcd.puts("gpio supported!");
    for (;;) {}
}
```

## Example Usage (I2C with PCF8574 expander):
```cpp
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "hd44780_pico/lcd.h"
#include "hd44780_pico/lcd_i2c.h"

int main()
{
    stdio_init_all();
    
    using namespace hd44780pico;
   
    // The user is responsible for managing the I2C, not the class unlike with the parallel GPIO implementation.
    i2c_init(i2c0, 400e3);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);
    
    /*
    Mapping in this instance correlates to which bit in the I2C data field corresponds to which LCD pin.
    In this case, the mapping is correct for the PCF8574 expander.
    */
    Mapping i2c_map{};
    i2c_map.rs = 0;
    i2c_map.rw = 1;
    i2c_map.e = 2;
    i2c_map.backlight = 3;
    i2c_map.d4 = 4;
    i2c_map.d5 = 5;
    i2c_map.d6 = 6;
    i2c_map.d7 = 7;
    
    LcdDisplayI2c lcd{i2c_map, i2c0, 0x27};

    lcd.init(BitMode::FOUR_BIT, LineMode::TWO_LINES_5_8);
    lcd.set_blink_enabled(true); 
    lcd.set_backlight_enabled(true);
    lcd.puts("i2c supported!");
    for (;;) {}
}
```

TBA. 
Rewrite in C++ of my previous simple-lcd-lib C library implementing classes to manipulate any HD44780-compatible screen via the Pico C/C++ Library. Now supports both 4-bit and 8-bit mode and different line/font modes. Currently supports GPIO parallel pin interface with future support for I2C planned.