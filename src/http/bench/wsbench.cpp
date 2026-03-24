#include "icy/buffer.h"
#include "icy/http/websocket.h"

#include "benchutil.h"

#include <array>
#include <cstdint>
#include <iostream>

using namespace icy;

namespace {

void completeHandshake(http::ws::WebSocketFramer& clientFramer,
                       http::ws::WebSocketFramer& serverFramer)
{
    http::Request request(http::Method::Get, "/bench");
    request.setHost("127.0.0.1");
    http::Response response;

    clientFramer.createClientHandshakeRequest(request);
    serverFramer.acceptServerRequest(request, response);
    if (!clientFramer.checkClientHandshakeResponse(response)) {
        std::cerr << "wsbench: handshake failed\n";
        std::exit(1);
    }
}

void benchWrite(bench::Reporter& reporter)
{
    http::ws::WebSocketFramer framer(http::ws::ClientSide);
    http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
    completeHandshake(framer, serverFramer);
    std::array<char, 1024> payload{};
    Buffer frame;
    frame.reserve(payload.size() + 32);

    constexpr uint64_t iterations = 100000;
    uint64_t bytes = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            frame.clear();
            DynamicBitWriter writer(frame);
            framer.writeFrame(payload.data(), payload.size(),
                              http::ws::SendFlags::Binary, writer);
            bytes += frame.size();
        }
    });

    reporter.add({
        .name = "websocket frame write",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations)},
    });
}

void benchRead(bench::Reporter& reporter)
{
    http::ws::WebSocketFramer writerFramer(http::ws::ClientSide);
    http::ws::WebSocketFramer readerFramer(http::ws::ServerSide);
    completeHandshake(writerFramer, readerFramer);
    std::array<char, 1024> payload{};
    Buffer frame;
    frame.reserve(payload.size() + 32);

    {
        DynamicBitWriter writer(frame);
        writerFramer.writeFrame(payload.data(), payload.size(),
                                http::ws::SendFlags::Binary, writer);
    }

    constexpr uint64_t iterations = 100000;
    uint64_t bytes = 0;
    uint64_t checksum = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        checksum = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            BitReader reader(frame);
            char* decoded = nullptr;
            const uint64_t payloadLen = readerFramer.readFrame(reader, decoded);
            bytes += payloadLen;
            checksum += static_cast<unsigned char>(decoded[0]);
        }
    });

    reporter.add({
        .name = "websocket frame read",
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
