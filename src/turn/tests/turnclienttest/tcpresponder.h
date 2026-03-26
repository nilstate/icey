#pragma once
#include "icy/logger.h"
#include "icy/net/tcpsocket.h"
#include "icy/time.h"
#include "icy/util.h"
#include <string>


using namespace std;


namespace icy {


class TCPResponder : public net::SocketAdapter
{
public:
    int id;
    net::Address relayedAddr;
    net::TCPSocket socket;
    Timer timer;

    TCPResponder(int id)
        : id(id)
        , timer(1000, 1000)
    {
        LDebug(id, ": Creating");

        socket.addReceiver(this);
    }

    virtual ~TCPResponder()
    {
        LDebug(id, ": Destroying");
        socket.removeReceiver(this);
    }

    void connect(const net::Address& relayAddr)
    {
        LDebug(id, ": Starting on: ", relayAddr);

        try {
            this->relayedAddr = relayAddr;

            // Since we extend SocketAdapter socket callbacks
            // will be received below.
            socket.connect(relayAddr);
        } catch (std::exception& exc) {
            LError(id, ": ERROR: ", exc.what());
            assert(false);
        }
    }

    void stop()
    {
        socket.close();
        timer.stop();
    }

    bool onSocketConnect(net::Socket&)
    {
        // Send some early media to client
        // sendPacketToInitiator();

        // Start the send timer
        // timer.Timeout += slot(this, &TCPResponder::onSendTimer);
        // timer.start();
        return false;
    }

    bool onSocketRecv(net::Socket&, const MutableBuffer& buffer, const net::Address& peerAddress)
    {
        LDebug(id, ": On recv: ", peerAddress, ": ", buffer.size());

        // Echo data back to client
        (void)socket.sendOwned(Buffer(bufferCast<const char*>(buffer),
                                      bufferCast<const char*>(buffer) + buffer.size()));
        return false;
    }

    bool onSocketError(net::Socket&, const icy::Error& error)
    {
        LDebug(id, ": On error: ", error.message);
        return false;
    }

    bool onSocketClose(net::Socket&)
    {
        LDebug(id, ": On close");
        return false;
    }

};


} //  namespace icy
