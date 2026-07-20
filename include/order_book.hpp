#pragma once
#include <vector>
#include <map>
#include <optional>
#include <algorithm>   // for std::min
#include <cstdint>
#include "order.hpp"
#include "price_level.hpp"
#include "trade.hpp"

class OrderBook {
public:
    void add_resting(const Order& order) {
        auto& book = (order.side == Side::Buy) ? bids_ : asks_;
        book[order.price].price = order.price;
        book[order.price].orders.push_back(order);
    }

    std::vector<Trade> submit(Order incoming) {
        std::vector<Trade> trades;

        if (incoming.side == Side::Buy) {
            while (incoming.quantity > 0 && !asks_.empty()) {
                auto best = asks_.begin();
                if (best->first > incoming.price) break;
                match_at_level(incoming, best->second, trades);
                if (best->second.orders.empty())
                    asks_.erase(best);
            }
        } else {
            while (incoming.quantity > 0 && !bids_.empty()) {
                auto best = std::prev(bids_.end());
                if (best->first < incoming.price) break;
                match_at_level(incoming, best->second, trades);
                if (best->second.orders.empty())
                    bids_.erase(best);
            }
        }

        if (incoming.quantity > 0)
            add_resting(incoming);

        return trades;
    }

    std::optional<int64_t> best_bid() const {
        if (bids_.empty()) return std::nullopt;
        return bids_.rbegin()->first;
    }

    std::optional<int64_t> best_ask() const {
        if (asks_.empty()) return std::nullopt;
        return asks_.begin()->first;
    }

private:
    void match_at_level(Order& incoming, PriceLevel& level,
                        std::vector<Trade>& trades) {
        while (incoming.quantity > 0 && !level.orders.empty()) {
            Order& resting = level.orders.front();
            uint64_t fill = std::min(incoming.quantity, resting.quantity);

            if (incoming.side == Side::Buy)
                trades.push_back({incoming.id, resting.id, resting.price, fill});
            else
                trades.push_back({resting.id, incoming.id, resting.price, fill});

            incoming.quantity -= fill;
            resting.quantity  -= fill;

            if (resting.quantity == 0)
                level.orders.pop_front();
        }
    }

    std::map<int64_t, PriceLevel> bids_;
    std::map<int64_t, PriceLevel> asks_;
};