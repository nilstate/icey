#include "icy/buffer.h"
#include "icy/stun/message.h"

#include <fstream>
#include <iterator>
#include <vector>

namespace {

void fuzzOne(const uint8_t* data, size_t size)
{
    static constexpr size_t kMessageHeaderSize = 20;
    const char* bytes = size ? reinterpret_cast<const char*>(data) : "";
    size_t remaining = size;
    size_t offset = 0;

    while (remaining >= kMessageHeaderSize) {
        icy::stun::Message message;
        ssize_t consumed = 0;
        try {
            consumed = message.read(icy::constBuffer(bytes + offset, remaining));
        } catch (...) {
            return;
        }

        if (consumed <= 0)
            return;

        icy::Buffer encoded;
        try {
            message.write(encoded);
            auto copy = message;
            (void)copy.toString();
        } catch (...) {
        }

        offset += static_cast<size_t>(consumed);
        remaining -= static_cast<size_t>(consumed);
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
