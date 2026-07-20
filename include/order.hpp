#pragma once
#include <cstdint>

enum class Side { Buy, Sell };

struct Order {
    uint64_t id;         // unique identifier, used for cancel/modify
    Side side;           // Buy or Sell
    int64_t price;       // in ticks (e.g. cents), never floating point
    uint64_t quantity;   // remaining quantity to fill
    uint64_t timestamp;  // arrival order, for time priority
};