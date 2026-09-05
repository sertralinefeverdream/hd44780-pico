#ifndef HD44780_LCD_TRANSPORT_H
#define HD44780_LCD_TRANSPORT_H

#include <cstdint>
#include <string_view>

namespace hd44780pico {
    enum class OperationMode {
        FOUR_BIT,
        EIGHT_BIT,
    };
    
    enum class LineMode {
        ONE_LINE_5_8, 
        TWO_LINES_5_8,
        ONE_LINE_5_10,
    };
    
    enum class Direction{
        LEFT,
        RIGHT,
    };
    
    enum class ShiftType {
        SHIFT_CURSOR,
        SHIFT_DISPLAY,
    };
   
    class LcdDisplay {
    public:
        explicit LcdDisplay(OperationMode op_mode=OperationMode::FOUR_BIT, LineMode line_mode=LineMode::TWO_LINES_5_8);

        void clear_display();
        void return_home();
        void putc(const char c);
        void puts(const std::string_view s);
        void set_display_enabled(bool enabled);
        void set_backlight_enabled(bool enabled);
        void set_blink_enabled(bool enabled);
        void shift_cursor(int n);
        void shift_display(int n);
        void cursor_goto(const std::uint8_t col, const std::uint8_t row);
        
    protected:
        virtual const std::uint16_t buffer() const;
        virtual void send_buffer() const = 0;
        virtual void init_io() const = 0;

    private:
        enum Instruction : std::uint16_t { // RS, RW, DB7, DB6, DB5, DB4, DB3, DB2, DB1, DB0
            CLEAR_DISPLAY = 0b00'0000'0001,
            RETURN_HOME = 0b00'0000'0010,
            ENTRY_MODE_SET = 0b00'0000'0100,
            DISPLAY_CONTROL = 0b00'0000'1000,
            CURSOR_DISPLAY_SHIFT = 0b00'0001'0000,
            FUNCTION_SET = 0b00'0010'0000,
            SET_CGRAM = 0b00'0100'0000,
            SET_DDRAM = 0b00'1000'0000,
        };
        
        std::uint16_t buffer_{}; // 12 bits: BACKLIGHT, E, RS, RW, D0, D1, D2, D3, D4, D5, D6, D7
        bool backlight_enabled_{true};
        bool display_enabled_{true};
        bool blink_enabled_{true};
        bool shift_on_write_{false};
        ShiftType shift_type_{ShiftType::SHIFT_CURSOR};
        Direction cursor_dir_{Direction::RIGHT};
        OperationMode operation_mode_;
        LineMode line_mode_;

        void execute_instruction(std::uint16_t instr);
        void set_buffer_raw(uint16_t buf);
        void pulse_enable();
        void init_sequence();
   
    };
}

#endif // HD44780_LCD_TRANSPORT_HH