#include "icy/signal.h"
#include "icy/time.h"

#include <cstdint>
#include <iostream>

using namespace icy;

namespace {

struct Counter
{
    void increment(uint64_t& value) { ++value; }
    void incrementConst(uint64_t& value) const { ++value; }
    static void incrementStatic(uint64_t& value) { ++value; }
};

void incrementFree(uint64_t& value)
{
    ++value;
}

template <typename Attach>
void runBenchmark(const char* name, Attach&& attach)
{
    Signal<void(uint64_t&)> signal;
    Counter counter;
    attach(signal, counter);

    constexpr uint64_t iterations = 999999;
    uint64_t value = 0;
    const uint64_t start = time::hrtime();
    for (uint64_t index = 0; index < iterations; ++index)
        signal.emit(value);
    const uint64_t end = time::hrtime();

    std::cout << name << ": "
              << ((end - start) * 1.0 / iterations) << "ns per emission"
              << " (sz=" << sizeof(signal) << ")\n";
}

} // namespace

int main()
{
    runBenchmark("signal class member", [](auto& signal, auto& counter) {
        signal += slot(&counter, &Counter::increment);
    });
    runBenchmark("signal const class member", [](auto& signal, auto& counter) {
        signal += slot(&counter, &Counter::incrementConst);
    });
    runBenchmark("signal static member", [](auto& signal, auto&) {
        signal += slot(&Counter::incrementStatic);
    });
    runBenchmark("signal free function", [](auto& signal, auto&) {
        signal += incrementFree;
    });
    return 0;
}
