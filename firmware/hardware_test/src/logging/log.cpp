#include "log.hpp"
#include "services/console.hpp"


#include <Arduino.h>



void log_info(const char* message)
{
    Serial.println(message);
    console_println(message);
}
