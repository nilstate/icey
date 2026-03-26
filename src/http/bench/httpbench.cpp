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

http::Request makeRequest()
{
    http::Request request(http::Method::Post, "/api/v1/items?limit=10");
    request.setHost("127.0.0.1", 8080);
    request.setKeepAlive(true);
    request.setContentType("application/json");
    request.setContentLength(26);
    request.add("User-Agent", "icey-bench");
    request.add("Accept", "application/json");
    return request;
}

http::Response makeResponse()
{
    http::Response response(http::StatusCode::OK);
    response.setContentType("application/json");
    response.setContentLength(27);
    response.setKeepAlive(true);
    response.add("Server", "icey-bench");
    response.add("X-Bench", "1");
    return response;
}

const std::string& requestWire()
{
    static const std::string wire =
        "POST /api/v1/items?limit=10 HTTP/1.1\r\n"
        "Host: 127.0.0.1:8080\r\n"
        "User-Agent: icey-bench\r\n"
        "Accept: application/json\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 26\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "{\"name\":\"icey\",\"count\":42}";
    return wire;
}

void benchRequestWrite(bench::Reporter& reporter)
{
    const http::Request request = makeRequest();
    Buffer buffer;
    buffer.reserve(512);

    constexpr uint64_t iterations = 200000;
    uint64_t bytes = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            buffer.clear();
            request.write(buffer);
            bytes += buffer.size();
        }
    });

    reporter.add({
        .name = "http request write",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations)},
    });
}

void benchResponseWrite(bench::Reporter& reporter)
{
    const http::Response response = makeResponse();
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

void benchRequestResponseCycle(bench::Reporter& reporter)
{
    const std::string& wire = requestWire();
    const http::Response response = makeResponse();
    Buffer responseBuffer;
    responseBuffer.reserve(512);

    constexpr uint64_t iterations = 100000;
    uint64_t bytes = 0;
    uint64_t checksum = 0;

    const auto measurement = bench::measure(reporter.options(), iterations, [&] {
        bytes = 0;
        checksum = 0;
        for (uint64_t index = 0; index < iterations; ++index) {
            http::Request request;
            http::Parser parser(&request);
            const auto result = parser.parse(wire.data(), wire.size());
            if (!result.ok() || !result.messageComplete) {
                std::cerr << "httpbench: cycle parse failed\n";
                std::exit(1);
            }

            responseBuffer.clear();
            response.write(responseBuffer);

            bytes += result.bytesConsumed + responseBuffer.size();
            checksum += request.getMethod().size() + request.getURI().size()
                + responseBuffer.size();
        }
    });

    reporter.add({
        .name = "http request-response cycle",
        .measurement = measurement,
        .metrics = {bench::metric("bytes", bytes / iterations),
                    bench::metric("checksum", checksum)},
    });
}

} // namespace

int main(int argc, char** argv)
{
    bench::Reporter reporter(argc, argv);
    benchRequestWrite(reporter);
    benchResponseWrite(reporter);
    benchRequestResponseCycle(reporter);
    return reporter.finish();
}
