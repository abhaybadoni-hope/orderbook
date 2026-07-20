#pragma once
#include <deque>
#include <cstdint>
#include "order.hpp"

struct PriceLevel {
    int64_t price;              // the price all these orders share (ticks)
    std::deque<Order> orders;   // FIFO: front = oldest = fills first

    uint64_t total_quantity() const {
        uint64_t sum = 0;
        for (const auto& o : orders) sum += o.quantity;
        return sum;
    }
};