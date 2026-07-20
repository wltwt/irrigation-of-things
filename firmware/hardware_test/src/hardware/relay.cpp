#include "relay.hpp"

#include <Arduino.h>

void relay_init(const RelayConfig& relay)
{
    digitalWrite(
        relay.pin,
        relay.active_low ? HIGH : LOW
    );

    pinMode(relay.pin, OUTPUT);
}

void relay_on(const RelayConfig& relay)
{
    digitalWrite(
        relay.pin,
        relay.active_low ? LOW : HIGH
    );
}

void relay_off(const RelayConfig& relay)
{
    digitalWrite(
        relay.pin,
        relay.active_low ? HIGH : LOW
    );
}

bool relay_is_on(const RelayConfig& relay)
{
    const int level = digitalRead(relay.pin);

    return relay.active_low
        ? level == LOW
        : level == HIGH;
}