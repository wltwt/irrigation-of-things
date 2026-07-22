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
    // Relay ON commands opening. The internal end stop disconnects the motor
    // at full travel, while the relay remains ON to hold the open command.
    relay_on(board.valve);
}

void valve_close()
{
    // Relay OFF commands closing. The internal end stop disconnects the motor
    // when the valve reaches the fully closed position.
    relay_off(board.valve);
}

bool valve_status()
{
    return relay_is_on(board.valve);
}
