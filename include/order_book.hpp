#pragma once
#include <map>
#include <optional>
#include <cstdint>
#include "order.hpp"
#include "price_level.hpp"

class OrderBook {
public:
    // Rest an order on the appropriate side (no matching yet — that's next phase).
    void add_resting(const Order& order) {
        auto& book = (order.side == Side::Buy) ? bids_ : asks_;
        book[order.price].price = order.price;
        book[order.price].orders.push_back(order);
    }

    // Best bid = highest buy price. rbegin() is the largest key.
    std::optional<int64_t> best_bid() const {
        if (bids_.empty()) return std::nullopt;
        return bids_.rbegin()->first;
    }

    // Best ask = lowest sell price. begin() is the smallest key.
    std::optional<int64_t> best_ask() const {
        if (asks_.empty()) return std::nullopt;
        return asks_.begin()->first;
    }

private:
    std::map<int64_t, PriceLevel> bids_;   // buy orders, keyed by price
    std::map<int64_t, PriceLevel> asks_;   // sell orders, keyed by price
};