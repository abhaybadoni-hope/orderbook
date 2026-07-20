#include <iostream>
#include "order.hpp"
#include "order_book.hpp"

void show(OrderBook& book) {
    std::cout << "  best bid: "
              << (book.best_bid() ? std::to_string(*book.best_bid()) : "none")
              << " | best ask: "
              << (book.best_ask() ? std::to_string(*book.best_ask()) : "none")
              << "\n";
}

int main() {
    OrderBook book;

    book.submit(Order{1, Side::Buy, 10100, 10, 1});
    book.submit(Order{2, Side::Buy, 10150, 20, 2});
    std::cout << "After two buys:\n"; show(book);

    std::cout << "Cancel #2 (the 10150 bid):\n";
    book.cancel(2);
    show(book);   // best bid should drop back to 10100

    std::cout << "Cancel #99 (doesn't exist): "
              << (book.cancel(99) ? "removed" : "not found") << "\n";

    return 0;
}