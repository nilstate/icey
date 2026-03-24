#include "icy/http/parser.h"
#include "icy/http/request.h"
#include "icy/http/response.h"

#include "benchutil.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace icy;

namespace {

void benchParse(bench::Reporter& reporter)
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

    constexpr uint64_t iterations = 100000;
    uint64_t bytes = 0;
    uint64_t checksum = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        checksum = 0;
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
    });

    reporter.add({
        .name = "http request parse",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations),
                    bench::metric("checksum", checksum)},
    });
}

void benchWrite(bench::Reporter& reporter)
{
    http::Response response(http::StatusCode::OK);
    response.setContentType("application/json");
    response.setContentLength(27);
    response.setKeepAlive(true);
    response.add("Server", "icey-bench");
    response.add("X-Bench", "1");

    Buffer buffer;
    buffer.reserve(512);

    constexpr uint64_t iterations = 200000;
    uint64_t bytes = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            buffer.clear();
            response.write(buffer);
            bytes += buffer.size();
        }
    });

    reporter.add({
        .name = "http response write",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations)},
    });
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);
    benchParse(reporter);
    benchWrite(reporter);
    return reporter.finish();
}
