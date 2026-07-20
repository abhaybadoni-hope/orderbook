#include <iostream>
#include "order.hpp"
#include "order_book.hpp"

int main() {
    OrderBook book;

    book.add_resting(Order{1, Side::Buy,  10100, 10, 1});  // buy @ 101.00
    book.add_resting(Order{2, Side::Buy,  10050, 20, 2});  // buy @ 100.50
    book.add_resting(Order{3, Side::Sell, 10200, 15, 3});  // sell @ 102.00
    book.add_resting(Order{4, Side::Sell, 10250,  5, 4});  // sell @ 102.50

    if (auto bid = book.best_bid())
        std::cout << "Best bid: " << *bid << "\n";
    if (auto ask = book.best_ask())
        std::cout << "Best ask: " << *ask << "\n";

    return 0;
}