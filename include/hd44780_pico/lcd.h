#ifndef HD44780_LCD_H
#define HD44780_LCD_H

#include <cstdint>
#include <cstddef>
#include <array>
#include <functional>
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
    
    enum class Direction {
        LEFT,
        RIGHT,
    };
    
    enum LcdPin : std::size_t {
        BACKLIGHT,
        E,
        RS,
        RW,
        D7,
        D6,
        D5,
        D4,
        D3,
        D2,
        D1,
        D0,
        NUM_PINS,
    };
    
    struct Mapping {
        static constexpr std::uint8_t unused_pin{UINT8_MAX};

        std::uint8_t backlight{unused_pin};
        std::uint8_t e{unused_pin};
        std::uint8_t rs{unused_pin};
        std::uint8_t rw{unused_pin};
        std::uint8_t d7{unused_pin};
        std::uint8_t d6{unused_pin};
        std::uint8_t d5{unused_pin};
        std::uint8_t d4{unused_pin};
        std::uint8_t d3{unused_pin};
        std::uint8_t d2{unused_pin};
        std::uint8_t d1{unused_pin};
        std::uint8_t d0{unused_pin};
        
        std::uint8_t operator[](std::size_t pin) const;
    };
    
    class LcdDisplay {
    public:
        explicit LcdDisplay(Mapping mapping_);

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
        virtual void send_buffer() const = 0;
        virtual void init_io() const = 0;
        std::uint16_t buffer() const;
        Mapping mapping() const;
        bool is_8_bit_enabled() const;
        bool is_two_lines_enabled() const;
        void init_sequence();

    private:
        std::uint16_t entry_mode_set_cmd_;
        std::uint16_t display_control_cmd_;
        std::uint16_t cursor_display_shift_cmd_;
        std::uint16_t function_set_cmd_;
        std::uint16_t buffer_{}; // Backlight, E, RS, RW, d7, d6, d5, d4, d3, d2, d1, d0
        Mapping mapping_;
        
        void clear_buffer_data_pins();
        void entry_mode_set(); 
        void display_control();
        void cursor_display_shift();
        void function_set();
        void set_ddram_addr(std::uint16_t addr);
        void write_ddram_data(std::uint16_t addr);
        void execute_cmd(std::uint16_t instr);
        void pulse_enable();
    };
}

#endif // HD44780_LCD_TRANSPORT_HH