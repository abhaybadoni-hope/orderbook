#include <iostream>
#include "order.hpp"
#include "price_level.hpp"

int main() {
    PriceLevel level{10150, {}};

    level.orders.push_back(Order{1, Side::Buy, 10150, 100, 1});
    level.orders.push_back(Order{2, Side::Buy, 10150, 50, 2});

    std::cout << "Price " << level.price
              << " has " << level.orders.size() << " orders"
              << ", total qty " << level.total_quantity() << "\n";

    std::cout << "First to fill: Order #" << level.orders.front().id << "\n";
    return 0;
}