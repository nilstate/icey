#include "icy/packet.h"
#include "icy/packetstream.h"
#include "icy/time.h"

#include <array>
#include <cstdint>
#include <iostream>

using namespace icy;

namespace {

struct BenchSource : PacketStreamAdapter
{
    PacketSignal emitter;

    BenchSource()
        : PacketStreamAdapter(emitter)
    {
    }

    void push(IPacket& packet)
    {
        emitter.emit(packet);
    }
};

struct PassthroughProcessor : PacketProcessor
{
    PacketSignal emitter;

    PassthroughProcessor()
        : PacketProcessor(emitter)
    {
    }

    void process(IPacket& packet) override
    {
        emitter.emit(packet);
    }
};

template <typename Configure>
void runBenchmark(const char* name, Configure&& configure)
{
    PacketStream stream(name);
    BenchSource source;
    uint64_t packets = 0;

    stream.attachSource(&source, false, false);
    configure(stream);
    stream.emitter += [&](IPacket&) { ++packets; };
    stream.start();

    std::array<char, 256> payload{};
    RawPacket packet(payload.data(), payload.size());

    constexpr uint64_t iterations = 1000000;
    const uint64_t start = time::hrtime();
    for (uint64_t index = 0; index < iterations; ++index)
        source.push(packet);
    const uint64_t end = time::hrtime();

    std::cout << name << ": "
              << ((end - start) * 1.0 / iterations) << "ns per packet"
              << " (packets=" << packets << ")\n";

    stream.close();
}

} // namespace

int main()
{
    runBenchmark("packetstream passthrough", [](PacketStream&) {});
    runBenchmark("packetstream single processor", [](PacketStream& stream) {
        stream.attach(new PassthroughProcessor, 1, true);
    });
    runBenchmark("packetstream triple processor", [](PacketStream& stream) {
        stream.attach(new PassthroughProcessor, 1, true);
        stream.attach(new PassthroughProcessor, 2, true);
        stream.attach(new PassthroughProcessor, 3, true);
    });
    return 0;
}
