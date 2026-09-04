#include "../include/hd44780_lib/lcd.h"

using namespace hd44780pico;

LcdDisplay::LcdDisplay(OperationMode op_mode, LineMode line_mode)
    : operation_mode_{op_mode}
    , line_mode_{line_mode} {} 

