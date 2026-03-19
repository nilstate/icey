#pragma once

#include "icy/logger.h"
#include "icy/net/address.h"

#include <memory>


namespace icy {
namespace net {


template <typename SocketT>
class ClientSocketTest : public SocketAdapter
{
public:
    SocketT socket;
    Address address;
    bool passed;

    ClientSocketTest(short port)
        : address("127.0.0.1", port)
        , passed(false)
    {
        LTrace("Creating: ", port);
    }

    virtual ~ClientSocketTest()
    {
        // assert(socket.base().refCount() == 1);
        LTrace("Destroying");
    }

    void run()
    {
        socket.addReceiver(this);
        socket.connect(address);
    }

    void stop()
    {
        socket.removeReceiver(this);
        socket.close();
        //socket.shutdown();
    }

    virtual bool onSocketConnect(Socket& sock) override
    {
        LDebug("Connected");

        sock.send("client > server", 15);
        return false;
    }

    virtual bool onSocketRecv(Socket&, const MutableBuffer& buffer, const Address& peerAddress) override
    {
        std::string data(bufferCast<const char*>(buffer), buffer.size());
        LDebug("On recv:", data);
        // Check for return packet echoing sent data
        if (data == "client > server") {
            LTrace("Recv: Got Return Packet!");
            passed = true;
            stop();
        } else {
            LError("Unexpected data received:", data);
            stop();
        }
        return false;
    }

    virtual bool onSocketError(Socket&, const Error& error) override
    {
        // assert(sender == &socket);
        LDebug("On error:", error.message);
        return false;
    }

    virtual bool onSocketClose(Socket&) override
    {
        // The last callback to fire is the Closed signal
        // which notifies us the underlying libuv socket
        // handle is closed. Das is gut!
        LDebug("On closed");
        return false;
    }
};


} // namespace net
} // namespace icy
