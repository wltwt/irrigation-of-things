// valve.cpp

#include "valve.hpp"
#include "hardware/relay.hpp"
#include "board/board_config.hpp"

void valve_init()
{
    relay_init(board.valve);

    valve_close();
}

void valve_open()
{
    relay_on(board.valve);
}

void valve_close()
{
    relay_off(board.valve);
}

void valve_stop()
{
    relay_off(board.valve);
}

bool valve_status()
{
    return relay_is_on(board.valve);
}
