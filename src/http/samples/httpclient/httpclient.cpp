///
//
// icey
// Copyright (c) 2005, icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// HTTP Client
//
// Demonstrates making an outbound HTTP GET request using the icey
// HTTP client. Prints the response status, headers, and body.
//
// Usage: httpclient [url]
//   Default: http://example.com
//
/// @addtogroup http
/// @{


#include "icy/application.h"
#include "icy/http/client.h"
#include "icy/logger.h"

#include <iostream>
#include <string>


using namespace icy;


int main(int argc, char** argv)
{
    std::string url = argc > 1 ? argv[1] : "http://example.com";

    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    // Create an HTTP client connection with automatic socket type
    // selection (TCP for http://, SSL for https://)
    auto conn = http::createConnectionT<http::ClientConnection>(http::URL(url));

    conn->Headers += [](http::Response& res) {
        std::cout << "Status: " << static_cast<int>(res.getStatus()) << " " << res.getReason() << '\n';
        std::cout << "Headers:" << '\n';
        for (const auto& [name, value] : res) {
            std::cout << "  " << name << ": " << value << '\n';
        }
        std::cout << '\n';
    };

    conn->Payload += [](const MutableBuffer& buf) {
        std::cout.write(bufferCast<const char*>(buf), buf.size());
    };

    conn->Complete += [](const http::Response& res) {
        std::cout << '\n';
        std::cout << "Request complete." << '\n';
    };

    conn->Close += [](http::Connection& conn) {
        // Stop the event loop once the connection closes
        uv_stop(uv::defaultLoop());
    };

    // Send the GET request
    conn->submit();

    std::cout << "GET " << url << '\n';

    // Run the event loop until the request completes
    uv_run(uv::defaultLoop(), UV_RUN_DEFAULT);

    Logger::destroy();
    return 0;
}

/// @}
