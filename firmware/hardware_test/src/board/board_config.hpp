

struct RelayConfig {
    int pin;
    bool active_low;
};

struct SensorConfig {
    int adc_pin;
};

struct BoardConfig {
    RelayConfig pump;
    RelayConfig valve1;
    RelayConfig valve2;

    SensorConfig reservoir;
    SensorConfig moisture;
};

inline constexpr BoardConfig board {
    .pump = {18, true},
    .valve1 = {16, true},
    .valve2 = {17, true},

    .reservoir = {34},
    .moisture = {32},
};