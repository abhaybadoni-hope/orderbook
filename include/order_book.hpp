#pragma once
#include <unordered_map>
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
        index_[order.id] = {order.side, order.price};   // register location
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
        // Cancel a resting order by id. Returns true if it was found and removed.
    bool cancel(uint64_t order_id) {
        auto it = index_.find(order_id);
        if (it == index_.end()) return false;    // not on the book

        Location loc = it->second;
        auto& book = (loc.side == Side::Buy) ? bids_ : asks_;
        auto level_it = book.find(loc.price);
        if (level_it == book.end()) { index_.erase(it); return false; }

        auto& orders = level_it->second.orders;
        for (auto oit = orders.begin(); oit != orders.end(); ++oit) {
            if (oit->id == order_id) {
                orders.erase(oit);               // remove the order
                break;
            }
        }
        if (orders.empty())
            book.erase(level_it);                // clean up empty level

        index_.erase(it);                        // remove from index
        return true;
    }
    // Modify = cancel + resubmit. Returns trades from the new order.
    std::vector<Trade> modify(uint64_t order_id, Order new_order) {
        cancel(order_id);
        return submit(new_order);
    }

private:
    struct Location { Side side; int64_t price; };
    std::unordered_map<uint64_t, Location> index_; 
    
    // order id -> where it rests
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
                if (resting.quantity == 0) {
                index_.erase(resting.id);        // remove from index
                level.orders.pop_front();
            }
        }
    }

    std::map<int64_t, PriceLevel> bids_;
    std::map<int64_t, PriceLevel> asks_;
};