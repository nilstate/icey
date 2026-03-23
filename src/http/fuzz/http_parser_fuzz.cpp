#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"

#include <fstream>
#include <iterator>
#include <vector>

namespace {

void fuzzOne(const uint8_t* data, size_t size)
{
    static constexpr char kEmpty = '\0';
    const char* bytes = size ? reinterpret_cast<const char*>(data) : &kEmpty;

    try {
        icy::http::Request request;
        icy::http::Parser requestParser(&request);
        auto requestResult = requestParser.parse(bytes, size);
        (void)requestResult;
        if (requestResult.messageComplete) {
            (void)request.getMethod();
            (void)request.getURI();
            (void)request.getVersion();
        }
    } catch (...) {
    }

    try {
        icy::http::Response response;
        icy::http::Parser responseParser(&response);
        auto responseResult = responseParser.parse(bytes, size);
        (void)responseResult;
        if (responseResult.messageComplete) {
            (void)response.getStatus();
            (void)response.getReason();
            (void)response.getVersion();
        }
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
