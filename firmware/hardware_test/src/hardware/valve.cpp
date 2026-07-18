#include <Arduino.h>
#include "board/board_config.hpp"

void valve_init()
{
    pinMode(board.valve1.pin, OUTPUT);
    pinMode(board.valve2.pin, OUTPUT);

}



