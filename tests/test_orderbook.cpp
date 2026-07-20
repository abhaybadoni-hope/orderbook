#include <catch2/catch_test_macros.hpp>
#include "order.hpp"
#include "order_book.hpp"

TEST_CASE("A non-crossing order just rests on the book") {
    OrderBook book;
    auto trades = book.submit(Order{1, Side::Buy, 10000, 10, 1});
    REQUIRE(trades.empty());                 // nothing to match against
    REQUIRE(book.best_bid() == 10000);
    REQUIRE_FALSE(book.best_ask().has_value());
}

TEST_CASE("Two crossing orders produce one full trade") {
    OrderBook book;
    book.submit(Order{1, Side::Sell, 10000, 10, 1});
    auto trades = book.submit(Order{2, Side::Buy, 10000, 10, 2});

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].quantity == 10);
    REQUIRE(trades[0].price == 10000);
    REQUIRE_FALSE(book.best_ask().has_value());  // ask fully consumed
    REQUIRE_FALSE(book.best_bid().has_value());  // buy fully filled, didn't rest
}

TEST_CASE("A partial fill leaves the remainder resting") {
    OrderBook book;
    book.submit(Order{1, Side::Sell, 10000, 5, 1});   // only 5 available
    auto trades = book.submit(Order{2, Side::Buy, 10000, 12, 2}); // wants 12

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].quantity == 5);        // filled what was there
    REQUIRE(book.best_bid() == 10000);       // remaining 7 rests as a bid
    REQUIRE_FALSE(book.best_ask().has_value());
}

TEST_CASE("Time priority: earlier order at a price fills first") {
    OrderBook book;
    book.submit(Order{1, Side::Sell, 10000, 5, 1});   // older
    book.submit(Order{2, Side::Sell, 10000, 5, 2});   // newer, same price
    auto trades = book.submit(Order{3, Side::Buy, 10000, 5, 3});

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].sell_id == 1);         // the OLDER order filled, not #2
}

TEST_CASE("Price priority: a buy sweeps the cheapest ask first") {
    OrderBook book;
    book.submit(Order{1, Side::Sell, 10200, 5, 1});   // more expensive
    book.submit(Order{2, Side::Sell, 10100, 5, 2});   // cheaper
    auto trades = book.submit(Order{3, Side::Buy, 10200, 5, 3});

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].sell_id == 2);         // cheaper ask (#2) hit first
    REQUIRE(trades[0].price == 10100);       // executed at resting price
}

TEST_CASE("A sweep across multiple levels fills in price order") {
    OrderBook book;
    book.submit(Order{1, Side::Sell, 10100, 5, 1});
    book.submit(Order{2, Side::Sell, 10200, 5, 2});
    auto trades = book.submit(Order{3, Side::Buy, 10200, 8, 3});

    REQUIRE(trades.size() == 2);
    REQUIRE(trades[0].price == 10100);       // cheaper level first
    REQUIRE(trades[1].price == 10200);       // then the next level
    REQUIRE(trades[0].quantity == 5);
    REQUIRE(trades[1].quantity == 3);        // only 3 of the 8 left
}

TEST_CASE("Cancel removes a resting order") {
    OrderBook book;
    book.submit(Order{1, Side::Buy, 10100, 10, 1});
    book.submit(Order{2, Side::Buy, 10150, 10, 2});
    REQUIRE(book.best_bid() == 10150);

    REQUIRE(book.cancel(2) == true);         // remove the top bid
    REQUIRE(book.best_bid() == 10100);       // falls back to the next
}

TEST_CASE("Cancelling a non-existent order fails gracefully") {
    OrderBook book;
    REQUIRE(book.cancel(999) == false);      // no crash, just false
}

TEST_CASE("A cancelled order no longer matches") {
    OrderBook book;
    book.submit(Order{1, Side::Sell, 10000, 10, 1});
    book.cancel(1);
    auto trades = book.submit(Order{2, Side::Buy, 10000, 10, 2});
    REQUIRE(trades.empty());                 // nothing left to trade with
    REQUIRE(book.best_bid() == 10000);       // buy rests instead
}