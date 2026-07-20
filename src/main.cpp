#include <iostream>
#include "order.hpp"

int main() {
    Order o{1, Side::Buy, 10150, 100, 1};

    std::cout << "Order #" << o.id
              << " side=" << (o.side == Side::Buy ? "Buy" : "Sell")
              << " price=" << o.price
              << " qty=" << o.quantity << "\n";
    return 0;
}