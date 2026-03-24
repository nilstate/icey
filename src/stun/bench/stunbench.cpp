#include "icy/net/address.h"
#include "icy/stun/message.h"

#include "benchutil.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>

using namespace icy;

namespace {

stun::Message makeMessage()
{
    stun::Message message(stun::Message::Request, stun::Message::Allocate);
    message.setTransactionID("123456789012");

    message.add<stun::Username>().copyBytes("benchmark-user", 14);
    message.add<stun::Realm>().copyBytes("icey.local", 10);
    message.add<stun::Nonce>().copyBytes("nonce-token", 11);
    message.add<stun::Software>().copyBytes("icey-bench", 10);
    message.add<stun::RequestedTransport>().setValue(17 << 24);
    message.add<stun::Lifetime>().setValue(600);
    message.add<stun::XorPeerAddress>().setAddress(net::Address("203.0.113.15", 3478));
    message.add<stun::Data>().copyBytes("0123456789abcdef", 16);

    return message;
}

void benchWrite(bench::Reporter& reporter)
{
    const stun::Message message = makeMessage();
    Buffer buffer;
    buffer.reserve(512);

    constexpr uint64_t iterations = 50000;
    uint64_t bytes = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            buffer.clear();
            message.write(buffer);
            bytes += buffer.size();
        }
    });

    reporter.add({
        .name = "stun encode",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations)},
    });
}

void benchRead(bench::Reporter& reporter)
{
    const stun::Message message = makeMessage();
    Buffer wire;
    message.write(wire);

    constexpr uint64_t iterations = 25000;
    uint64_t bytes = 0;
    uint64_t checksum = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        checksum = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            stun::Message parsed;
            const ssize_t consumed = parsed.read(constBuffer(wire));
            if (consumed <= 0) {
                std::cerr << "stunbench: parse failed\n";
                std::exit(1);
            }
            bytes += static_cast<uint64_t>(consumed);
            checksum += parsed.size();
        }
    });

    reporter.add({
        .name = "stun decode",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations),
                    bench::metric("checksum", checksum)},
    });
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);
    benchWrite(reporter);
    benchRead(reporter);
    return reporter.finish();
}
