#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/http/websocket.h"

#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

namespace {

icy::http::ws::WebSocketFramer makeReadyServerFramer()
{
    icy::http::ws::WebSocketFramer framer(icy::http::ws::ServerSide);
    icy::http::Request request;
    icy::http::Response response;
    request.set("Connection", "Upgrade");
    request.set("Upgrade", "websocket");
    request.set("Sec-WebSocket-Version", icy::http::ws::ProtocolVersion);
    request.set("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
    framer.acceptServerRequest(request, response);
    return framer;
}

void fuzzOne(const uint8_t* data, size_t size)
{
    constexpr size_t kMaxHeaderLength = 14;
    auto framer = makeReadyServerFramer();

    try {
        icy::Buffer encoded;
        encoded.reserve(size + kMaxHeaderLength);
        icy::DynamicBitWriter writer(encoded);
        const char* bytes = size ? reinterpret_cast<const char*>(data) : "";
        framer.writeFrame(bytes, size, icy::http::ws::SendFlags::Binary, writer);
    } catch (...) {
    }

    std::vector<char> frameStorage(size);
    if (size > 0)
        std::memcpy(frameStorage.data(), data, size);
    if (size > 1)
        frameStorage[1] = static_cast<char>(
            static_cast<unsigned char>(frameStorage[1]) | 0x80);

    try {
        icy::BitReader reader(icy::mutableBuffer(frameStorage.data(), frameStorage.size()));
        char* payload = nullptr;
        (void)framer.readFrame(reader, payload);
    } catch (...) {
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    fuzzOne(data, size);
    return 0;
}

#ifdef ICY_STANDALONE_FUZZER
int main(int argc, char** argv)
{
    for (int index = 1; index < argc; ++index) {
        std::ifstream input(argv[index], std::ios::binary);
        std::vector<uint8_t> bytes(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        LLVMFuzzerTestOneInput(bytes.data(), bytes.size());
    }
    return 0;
}
#endif
