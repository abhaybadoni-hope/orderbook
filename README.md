# orderbook

A limit order book and matching engine written in modern C++20, implementing
price-time priority matching — the core mechanism behind every electronic exchange.

## Highlights

- **Full matching engine**: limit orders, partial fills, and multi-level sweeps
- **Price-time priority**: best price first, FIFO within a price level
- **O(1) cancel/modify** via an order-id index
- **~2.7M orders/sec**, ~366 ns/order average latency (see Benchmark)
- **Tested** with Catch2: 9 cases covering matching, priority, and edge cases

## Design

The book keeps two sorted maps of price levels, one per side:

ASKS std::map<price, PriceLevel> best ask = begin() (lowest sell)
BIDS std::map<price, PriceLevel> best bid = rbegin() (highest buy)


Each `PriceLevel` holds a `std::deque<Order>` — a FIFO queue giving time
priority within a price. A separate `std::unordered_map<id, location>` indexes
every resting order so cancels and modifies avoid scanning the book.

### Complexity

| Operation        | Cost        | Why                                    |
|------------------|-------------|----------------------------------------|
| Add / match      | O(log P)    | locate price level in the sorted map   |
| Cancel           | O(1) avg    | id index → jump straight to the order  |
| Best bid / ask   | O(1)        | map extremes via begin() / rbegin()    |

*(P = number of distinct price levels)*

### Why a sorted map over a heap?

A heap gives cheap access to the best price but can't cancel an arbitrary
order or walk book depth efficiently. The sorted map supports the best-price
access matching needs *and* the arbitrary access that cancels need.

## Build & run

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/orderbook.exe     # demo driver
./build/tests.exe         # test suite
./build/benchmark.exe     # throughput / latency benchmark
```

## Benchmark

1,000,000 randomized orders through the engine (Release build):

Throughput: 2,726,233 orders/sec
Avg latency: 366 ns/order


*(Numbers from a single desktop machine; yours will vary.)*

## What I'd improve next

- **Preserve time priority on quantity-decrease modifies** — real exchanges
  keep an order's queue position when only reducing size; this implementation
  currently cancels and re-adds, sending it to the back.
- **Cache-friendlier layout** — replace node-based `std::map`/`std::deque` with
  contiguous, price-indexed arrays to cut cache misses on the hot path.
- **Latency percentiles (p50/p99)** — tail latency matters more than the
  average in trading; a histogram would be more honest than a single mean.
- **Order feed replay** — drive the engine from a recorded market data feed
  instead of synthetic random orders.

## Tech

C++20 · CMake · Ninja · Catch2
---
Built by abhay badoni (https://github.com/abhaybadoni-hope)
