#include "scy/net/sslsocket.h"
#include "scy/net/tcpsocket.h"

#include <algorithm>


namespace scy {
namespace net {


/// The TCP echo server accepts a template argument
/// of either a TCPSocket or a SSLSocket.
template <class SocketT>
class EchoServer : public SocketAdapter
{
public:
    typename SocketT::Ptr server;
    typename Socket::Vec sockets;

    EchoServer()
        : server(std::make_shared<SocketT>())
    {
    }

    ~EchoServer()
    {
        shutdown();
    }

    void start(const std::string& host, uint16_t port)
    {
        auto ssl = dynamic_cast<SSLSocket*>(server.get());
        if (ssl)
            ssl->useContext(SSLManager::instance().defaultServerContext());

        server->bind(Address(host, port));
        server->listen();
        server->AcceptConnection += slot(this, &EchoServer::onAcceptConnection);
    }

    void shutdown()
    {
        server->close();
        sockets.clear();
    }

    void onAcceptConnection(const TCPSocket::Ptr& socket)
    {
        socket->addReceiver(this);
        // socket->Recv += slot(this, &EchoServer::onClientSocketRecv);
        // socket->Error += slot(this, &EchoServer::onClientSocketError);
        // socket->Close += slot(this, &EchoServer::onClientSocketClose);
        sockets.push_back(socket);
        // LDebug("On accept: ", socket);
    }

    bool onSocketRecv(Socket& socket, const MutableBuffer& buffer, const Address& peerAddress)
    {
        // LDebug("On recv: ", &socket, ": ", buffer.str());

        // Echo it back
        socket.send(bufferCast<const char*>(buffer), buffer.size());
        return false;
    }

    bool onSocketError(Socket& socket, const Error& error)
    {
        LDebug("On error: ", error.err, ": ", error.message);
        return false;
    }

    bool onSocketClose(Socket& socket)
    {
        LDebug("On close");

        auto it = std::find_if(sockets.begin(), sockets.end(),
                               [&](const auto& s) { return s.get() == &socket; });
        if (it != sockets.end()) {
            LDebug("Removing: ", &socket);
            sockets.erase(it);
            return false;
        }
        LError("Unknown socket on close:", &socket);
        return false;
    }
};


// Some generic server types
using TCPEchoServer = EchoServer<TCPSocket>;
using SSLEchoServer = EchoServer<SSLSocket>;


} // namespace net
} // namespace scy
