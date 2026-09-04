#ifndef HD44780_LCD_TRANSPORT_H
#define HD44780_LCD_TRANSPORT_H

#include <cstdint>
#include <string_view>

#include "pico/stdlib.h"
#include "hardware/gpio.h"


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
    
    struct ParallelPinMap { 
        std::uint8_t rs{};
        std::uint8_t rw{};
        std::uint8_t e{};
        std::uint8_t backlight{};
        std::uint8_t d7{};
        std::uint8_t d6{};
        std::uint8_t d5{};
        std::uint8_t d4{};
        std::uint8_t d3{};
        std::uint8_t d2{};
        std::uint8_t d1{};
    }; 
    
    class LcdDisplay {
    public:
        explicit LcdDisplay(OperationMode op_mode=OperationMode::FOUR_BIT, LineMode line_mode=LineMode::TWO_LINES_5_8);
        void clear_display();
        void return_home();
        void putc(char c);
        void puts(std::string_view s);
        void set_display_enabled(bool enabled);
        void set_backlight_enabled(bool enabled);
        void set_cursor_blink(bool enabled);
        void cursor_goto(const std::uint8_t col, const std::uint8_t row);
        
    protected:
        bool backlight_enabled_{true};
        bool display_enabled_{true};
        bool cursor_blink_enabled_{true};
        bool shift_display_on_write_{false};
        ShiftType shift_type_{ShiftType::SHIFT_CURSOR};
        Direction cursor_dir_{Direction::RIGHT};
        OperationMode operation_mode_;
        LineMode line_mode_{LineMode::TWO_LINES_5_8};

        virtual void write_byte(const std::uint8_t byte, const bool pulse_enable=true) const = 0;
        virtual void write_nibble(const std::uint8_t nibble, const bool pulse_enable=true) const = 0;
        virtual void pulse_enable() const = 0;
        virtual void init_io() const = 0;
        virtual void init_sequence() const = 0;
    };
    
    class LcdDisplayParallel : public LcdDisplay {
    public:
        explicit LcdDisplayParallel(ParallelPinMap pin_map, OperationMode op_mode=OperationMode::FOUR_BIT, LineMode line_mode=LineMode::TWO_LINES_5_8);
    private:
        ParallelPinMap pin_map_;
    };
}

#endif // HD44780_LCD_TRANSPORT_HH