#pragma once
#include <cstdint>

struct Trade {
    uint64_t buy_id;    // resting or incoming buy order id
    uint64_t sell_id;   // resting or incoming sell order id
    int64_t  price;     // price the trade executed at
    uint64_t quantity;  // how much was traded
};