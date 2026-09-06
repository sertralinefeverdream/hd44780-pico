#ifndef HD44780_LCD_H
#define HD44780_LCD_H

#include <cstdint>
#include <string_view>

namespace hd44780pico {
    enum class LineMode {
        ONE_LINE_5_8, 
        TWO_LINES_5_8,
        ONE_LINE_5_10,
    };
    
    enum class BitMode {
        EIGHT_BIT, 
        FOUR_BIT,
    };
    
    enum class Direction{
        LEFT,
        RIGHT,
    };
    
    class LcdDisplay {
    public:
        explicit LcdDisplay();

        void init(BitMode bit_mode=BitMode::FOUR_BIT, LineMode line_mode=LineMode::TWO_LINES_5_8);
        void clear_display();
        void return_home();
        void putc(char c);
        void puts(std::string_view s);
        void set_display_enabled(bool enabled);
        void set_backlight_enabled(bool enabled);
        void set_blink_enabled(bool enabled);
        void set_cursor_enabled(bool enabled);
        void set_write_direction(Direction dir);
        void set_display_shift_enabled(bool enabled);
        void move_cursor(int n, Direction dir);
        void move_display(int n, Direction dir);
        void cursor_goto(std::uint16_t col, std::uint16_t row);
        
    protected:
        virtual const std::uint16_t buffer() const;
        virtual void send_buffer() const = 0;
        virtual void init_io() const = 0;

    private:
        uint16_t buffer_; // Backlight, E, RS, RW, d7, d6, d5, d4, d3, d2, d1, d0
        uint16_t entry_mode_set_cmd_;
        uint16_t display_control_cmd_;
        uint16_t cursor_display_shift_cmd_;
        uint16_t function_set_cmd_;
        
        void entry_mode_set(); 
        void display_control();
        void cursor_display_shift();
        void function_set();
        void set_ddram_addr(std::uint16_t addr);
        void write_ddram_data(std::uint16_t addr);
        void execute_cmd(std::uint16_t instr);
        void pulse_enable();
        void init_sequence();
    };
}

#endif // HD44780_LCD_TRANSPORT_HH