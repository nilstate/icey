#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"
#include "icy/time.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace icy;

namespace {

void benchParse()
{
    static const std::string requestWire =
        "POST /api/v1/items?limit=10 HTTP/1.1\r\n"
        "Host: 127.0.0.1:8080\r\n"
        "User-Agent: icey-bench\r\n"
        "Accept: application/json\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 26\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "{\"name\":\"icey\",\"count\":42}";

    constexpr uint64_t iterations = 250000;
    uint64_t bytes = 0;
    uint64_t checksum = 0;

    const uint64_t start = time::hrtime();
    for (uint64_t index = 0; index < iterations; ++index) {
        http::Request request;
        http::Parser parser(&request);
        auto result = parser.parse(requestWire.data(), requestWire.size());
        if (!result.ok() || !result.messageComplete) {
            std::cerr << "httpparsebench: parse failed\n";
            std::exit(1);
        }
        bytes += result.bytesConsumed;
        checksum += request.getMethod().size() + request.size();
    }
    const uint64_t end = time::hrtime();

    std::cout << "http request parse: "
              << ((end - start) * 1.0 / iterations) << "ns per parse"
              << " (bytes=" << bytes / iterations
              << ", checksum=" << checksum << ")\n";
}

void benchWrite()
{
    http::Response response(http::StatusCode::OK);
    response.setContentType("application/json");
    response.setContentLength(27);
    response.setKeepAlive(true);
    response.add("Server", "icey-bench");
    response.add("X-Bench", "1");

    Buffer buffer;
    buffer.reserve(512);

    constexpr uint64_t iterations = 500000;
    uint64_t bytes = 0;

    const uint64_t start = time::hrtime();
    for (uint64_t index = 0; index < iterations; ++index) {
        buffer.clear();
        response.write(buffer);
        bytes += buffer.size();
    }
    const uint64_t end = time::hrtime();

    std::cout << "http response write: "
              << ((end - start) * 1.0 / iterations) << "ns per write"
              << " (bytes=" << bytes / iterations << ")\n";
}

} // namespace

int main()
{
    benchParse();
    benchWrite();
    return 0;
}
