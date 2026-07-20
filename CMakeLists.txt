cmake_minimum_required(VERSION 3.16)
project(orderbook CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

add_executable(orderbook src/main.cpp)
target_include_directories(orderbook PRIVATE include)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests tests/test_orderbook.cpp)
target_include_directories(tests PRIVATE include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

enable_testing()
add_test(NAME orderbook_tests COMMAND tests)

add_executable(benchmark src/benchmark.cpp)
target_include_directories(benchmark PRIVATE include)
