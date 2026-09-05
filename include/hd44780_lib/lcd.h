#ifndef HD44780_LCD_TRANSPORT_H
#define HD44780_LCD_TRANSPORT_H

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

        void init(bool enable_8_bit, LineMode mode=LineMode::TWO_LINES_5_8);
        void clear_display();
        void return_home();
        void putc(const char c);
        void puts(const std::string_view s);
        void set_display_enabled(bool enabled);
        void set_backlight_enabled(bool enabled);
        void set_blink_enabled(bool enabled);
        void set_cursor_enabled(bool enabled);
        void set_write_direction(Direction dir);
        void set_display_shift_enabled(bool enabled);
        void move_cursor(int n, Direction dir);
        void move_display(int n, Direction dir);
        void cursor_goto(const std::uint8_t col, const std::uint8_t row);
        
    protected:
        virtual const std::uint16_t buffer() const;
        virtual void send_buffer() const = 0;
        virtual void init_io() const = 0;

    private:
        uint16_t buffer_; // Backlight, E, RS, RW, d7, d6, d5, d4, d3, d2, d1, d0
        uint16_t entry_mode_set_;
        uint16_t display_control_;
        uint16_t cursor_display_shift_;
        uint16_t function_set_;
        
        void execute_entry_mode_set(); 
        void execute_display_control();
        void execute_cursor_display_shift();
        void execute_function_set();
        void execute_instruction(std::uint16_t instr);
        void pulse_enable();
        void init_sequence();
    };
}

#endif // HD44780_LCD_TRANSPORT_HH