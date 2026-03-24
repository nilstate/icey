#include "icy/packet.h"
#include "icy/packetstream.h"

#include "benchutil.h"

#include <array>
#include <cstdint>

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
void runBenchmark(bench::Reporter& reporter, const char* name, Configure&& configure)
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

    constexpr uint64_t iterations = 100000;
    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        packets = 0;
        for (uint64_t index = 0; index < iterations; ++index)
            source.push(packet);
    });

    reporter.add({
        .name = name,
        .measurement = measurement,
        .metrics = {bench::metric("packets", packets),
                    bench::metric("payload_bytes", static_cast<uint64_t>(payload.size()))},
    });

    stream.close();
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);

    runBenchmark(reporter, "packetstream passthrough", [](PacketStream&) {});
    runBenchmark(reporter, "packetstream single processor", [](PacketStream& stream) {
        stream.attach(new PassthroughProcessor, 1, true);
    });
    runBenchmark(reporter, "packetstream triple processor", [](PacketStream& stream) {
        stream.attach(new PassthroughProcessor, 1, true);
        stream.attach(new PassthroughProcessor, 2, true);
        stream.attach(new PassthroughProcessor, 3, true);
    });
    return reporter.finish();
}
