#include <iostream>
#include "order.hpp"
#include "order_book.hpp"

void print_trades(const std::vector<Trade>& trades) {
    for (const auto& t : trades)
        std::cout << "  TRADE: buy#" << t.buy_id << " x sell#" << t.sell_id
                  << " qty " << t.quantity << " @ " << t.price << "\n";
}

int main() {
    OrderBook book;

    book.submit(Order{1, Side::Sell, 10200, 10, 1});
    book.submit(Order{2, Side::Sell, 10250,  5, 2});

    std::cout << "Buy 12 @ 10250:\n";
    auto trades = book.submit(Order{3, Side::Buy, 10250, 12, 3});
    print_trades(trades);

    std::cout << "Best ask now: "
              << (book.best_ask() ? std::to_string(*book.best_ask()) : "none")
              << "\n";
    return 0;
}