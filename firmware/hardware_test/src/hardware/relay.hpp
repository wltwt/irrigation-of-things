// relay.hpp

#pragma once

#include "board/board_config.hpp"

void relay_init(const RelayConfig& relay);
void relay_on(const RelayConfig& relay);
void relay_off(const RelayConfig& relay);
bool relay_is_on(const RelayConfig& relay);