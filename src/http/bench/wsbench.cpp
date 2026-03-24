#include "icy/buffer.h"
#include "icy/http/websocket.h"
#include "icy/time.h"

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

void benchWrite()
{
    http::ws::WebSocketFramer framer(http::ws::ClientSide);
    http::ws::WebSocketFramer serverFramer(http::ws::ServerSide);
    completeHandshake(framer, serverFramer);
    std::array<char, 1024> payload{};
    Buffer frame;
    frame.reserve(payload.size() + 32);

    constexpr uint64_t iterations = 250000;
    uint64_t bytes = 0;

    const uint64_t start = time::hrtime();
    for (uint64_t index = 0; index < iterations; ++index) {
        frame.clear();
        DynamicBitWriter writer(frame);
        framer.writeFrame(payload.data(), payload.size(),
                          http::ws::SendFlags::Binary, writer);
        bytes += frame.size();
    }
    const uint64_t end = time::hrtime();

    std::cout << "websocket frame write: "
              << ((end - start) * 1.0 / iterations) << "ns per frame"
              << " (bytes=" << bytes / iterations << ")\n";
}

void benchRead()
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

    constexpr uint64_t iterations = 250000;
    uint64_t bytes = 0;
    uint64_t checksum = 0;

    const uint64_t start = time::hrtime();
    for (uint64_t index = 0; index < iterations; ++index) {
        BitReader reader(frame);
        char* decoded = nullptr;
        const uint64_t payloadLen = readerFramer.readFrame(reader, decoded);
        bytes += payloadLen;
        checksum += static_cast<unsigned char>(decoded[0]);
    }
    const uint64_t end = time::hrtime();

    std::cout << "websocket frame read: "
              << ((end - start) * 1.0 / iterations) << "ns per frame"
              << " (bytes=" << bytes / iterations
              << ", checksum=" << checksum << ")\n";
}

} // namespace

int main()
{
    benchWrite();
    benchRead();
    return 0;
}
