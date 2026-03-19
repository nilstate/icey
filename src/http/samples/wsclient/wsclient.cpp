///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
// WebSocket Client
//
// Demonstrates a WebSocket client connection using the Icey HTTP
// module. Connects to a WebSocket server, sends a message, and prints
// any data received back.
//
// Usage: wsclient [url] [message]
//   Default: ws://echo.websocket.events "Hello from Icey!"
//
// To test locally, run the echoserver sample first and connect to
// ws://localhost:1337 instead.
//
/// @addtogroup http
/// @{


#include "icy/application.h"
#include "icy/http/client.h"
#include "icy/http/websocket.h"
#include "icy/logger.h"

#include <iostream>
#include <string>


using namespace icy;


int main(int argc, char** argv)
{
    std::string url = argc > 1 ? argv[1] : "ws://echo.websocket.events";
    std::string message = argc > 2 ? argv[2] : "Hello from Icey!";

    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Debug));

    // createConnectionT handles ws:// and wss:// schemes automatically,
    // installing a WebSocket ConnectionAdapter on the underlying TCP/SSL socket
    auto conn = http::createConnectionT<http::ClientConnection>(http::URL(url));

    conn->Connect += [&]() {
        std::cout << "Connected to " << url << '\n';
        std::cout << "Sending: " << message << '\n';

        // Send a text frame
        conn->send(message.c_str(), message.size(), http::ws::Text);
    };

    conn->Payload += [](const MutableBuffer& buf) {
        std::string data(bufferCast<const char*>(buf), buf.size());
        std::cout << "Received: " << data << '\n';
    };

    conn->Close += [](http::Connection& conn) {
        std::cout << "Connection closed." << '\n';
        uv_stop(uv::defaultLoop());
    };

    // Initiate the WebSocket handshake
    conn->send();

    std::cout << "Connecting to " << url << "..." << '\n';

    // Run the event loop; press Ctrl-C to exit
    waitForShutdown([&](void*) {
        conn->close();
    });

    Logger::destroy();
    return 0;
}
