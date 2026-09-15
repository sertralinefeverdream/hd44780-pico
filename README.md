# hd44780-pico

- A C++ library implementing classes to manipulate any HD44780-compatible screen via the Pico C/C++ SDK. Assumes a 16x2 character LCD but theoretically should work with any dimensions. 
- Supports:
    - Parallel GPIO (4-bit mode and 8-bit mode)
    - I2C via PCF8574-like GPIO expanders (4-bit mode only)
    - Different line modes (5x8 one-line, 5x8 two-lines, 5x10 one-line)
    
## Supported Lcd Methods
|Function|Description|
|--------|-----------|
|`void init (BitMode bit_mode, LineMode line_mode)`| Initialise the lcd display specifying bit mode (`BitMode::FOUR_BIT` / `BitMode::EIGHT_BIT`) and what type of line mode (`LineMode::ONE_LINE_5_8` / `LineMode::ONE_LINE_5_10` / `LineMode::TWO_LINES_5_8`).|
|`void clear_display()`| Trivial. |
| `void return_home()` | Return the cursor back to (0, 0) |
| `void putc(char c)` | Print a character to the cursor's current position and advance the cursor by 1. |
| `void puts(std::string_view s)` | Print a string to the cursor's current position. |
| `void set_display_enabled(bool enabled)` | Enable or disable the display. |
| `void set_backlight_enabled(bool enabled)` | Trivial. |
| `void set_blink_enabled(bool enabled)` | Enable or disable the cursor blinking.
| `void set_write_direction(Direction dir)` | Set the direction that characters are written. Either left-to-right (`Direction::RIGHT`) or right-to-left (`Direction::LEFT`)|
| `void set_display_shift_enabled(bool enabled)` | Enable or disable the behaviour where the display moves as characters are printed to the LCD display |
|`void move_cursor(int n, Direction dir)`| Move the cursor in direction `dir` by `n` times. |
|`void cursor_goto(std::uint16_t col, std::uint16_t row)`| Move cursor to position at column `col` and row `row`. *NOTE: This method assumes a standard 16x2 display and the column and row semantics may not apply to displays of other dimensions*|

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