#include "icy/signal.h"
#include "icy/time.h"

#include "benchutil.h"

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
void runBenchmark(bench::Reporter& reporter, const char* name, Attach&& attach)
{
    Signal<void(uint64_t&)> signal;
    Counter counter;
    attach(signal, counter);

    constexpr uint64_t iterations = 999999;
    uint64_t value = 0;
    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        value = 0;
        for (uint64_t index = 0; index < iterations; ++index)
            signal.emit(value);
    });

    reporter.add({
        .name = name,
        .measurement = measurement,
        .metrics = {bench::metric("size", static_cast<uint64_t>(sizeof(signal))),
                    bench::metric("value", value)},
    });
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);

    runBenchmark(reporter, "signal class member", [](auto& signal, auto& counter) {
        signal += slot(&counter, &Counter::increment);
    });
    runBenchmark(reporter, "signal const class member", [](auto& signal, auto& counter) {
        signal += slot(&counter, &Counter::incrementConst);
    });
    runBenchmark(reporter, "signal static member", [](auto& signal, auto&) {
        signal += slot(&Counter::incrementStatic);
    });
    runBenchmark(reporter, "signal free function", [](auto& signal, auto&) {
        signal += incrementFree;
    });
    return reporter.finish();
}
