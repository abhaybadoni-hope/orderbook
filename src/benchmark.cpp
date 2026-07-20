#include <iostream>
#include <chrono>
#include <random>
#include "order.hpp"
#include "order_book.hpp"

int main() {
    const int N = 1'000'000;              // number of orders to process

    // Pre-generate random orders so we time the ENGINE, not the RNG.
    std::mt19937_64 rng(42);              // fixed seed = reproducible
    std::uniform_int_distribution<int64_t> price_dist(9900, 10100);
    std::uniform_int_distribution<uint64_t> qty_dist(1, 100);
    std::uniform_int_distribution<int> side_dist(0, 1);

    std::vector<Order> orders;
    orders.reserve(N);
    for (int i = 0; i < N; ++i) {
        orders.push_back(Order{
            static_cast<uint64_t>(i + 1),
            side_dist(rng) ? Side::Buy : Side::Sell,
            price_dist(rng),
            qty_dist(rng),
            static_cast<uint64_t>(i + 1)
        });
    }

    // Time only the submission loop.
    OrderBook book;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto& o : orders)
        book.submit(o);
    auto end = std::chrono::high_resolution_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double seconds = ns / 1e9;
    double per_sec = N / seconds;

    std::cout << "Processed " << N << " orders in " << seconds << " s\n";
    std::cout << "Throughput: " << static_cast<uint64_t>(per_sec) << " orders/sec\n";
    std::cout << "Avg latency: " << (ns / N) << " ns/order\n";
    return 0;
}
