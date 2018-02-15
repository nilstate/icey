///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "scy/net/tcpsocket.h"
#include "scy/logger.h"


using std::endl;


namespace scy {
namespace net {


TCPSocket::TCPSocket(uv::Loop* loop)
    : Stream(loop)
{
    // LTrace("Create")
    init();
}


TCPSocket::~TCPSocket()
{
    // LTrace("Destroy")
    close();
}


void TCPSocket::init()
{
    if (initialized())
        return;

    // LTrace("Init")
    if (!get())
        Stream::reset();
    Stream::init(&uv_tcp_init_ex, _af);
    get()->data = this;
}


void TCPSocket::reset()
{
    Stream::reset();
    init();
    get()->data = this;
}


void TCPSocket::connect(const net::Address& peerAddress)
{
    // LTrace("Connecting to", peerAddress)
    init();

    uv::createRequest<uv::ConnectReq>([ptr = context()](const uv::BasicEvent& event) {
        if (!ptr->deleted) {
            auto handle = reinterpret_cast<TCPSocket*>(ptr->handle);
            if (event.status)
                handle->setUVError(event.status, "TCP connection failed");
            else
                handle->onConnect();
        }
    }).connect(get(), peerAddress.addr());

    // auto wrap = new ConnectReq();
    // wrap->callback = [ptr = context()](const uv::BasicEvent& event) {
    //     if (!ptr->deleted) {
    //         auto handle = reinterpret_cast<TCPSocket*>(ptr->handle);
    //         if (event.status)
    //             handle->setUVError(event.status, "TCP connection failed");
    //         else
    //             handle->onConnect();
    //     }
    // };
    // wrap->connect(get(), peerAddress.addr());

    // auto wrap = new ConnectReq(this);
    // wrap->invoke(&uv_tcp_connect, &wrap->req, get(), peerAddress.addr(),
    //     [](uv_connect_t* req, int status) {
    //         auto wrap = static_cast<ConnectReq*>(req->data);
    //         auto handle = wrap->handle<TCPSocket>();
    //         if (handle) {
    //             if (status == 0) {
    //                 handle->onConnect();
    //             }
    //             else {
    //                 // Error handled by callback proxy
    //                 handle->setUVError(status, "TCP connection failed");
    //             }
    //             // handle->onConnect(req, status);
    //         }
    //         else {
    //             LDebug("Dropping request for closed TCP socket")
    //         }
    //         delete wrap;
    //     });
}


void TCPSocket::connect(const std::string& host, uint16_t port)
{
    // LTrace("Connecting to", peerAddress)

    if (Address::validateIP(host)) {
        connect(Address(host, port));
    }
    else if (host == "localhost") {
        // NOTE: Forcefully translate localhost to 127.0.0.1 since
        // the DNS service returns 0.0.0.0 on some systems resulting
        // in connection failure.
        connect(Address("127.0.0.1", port));
    }
    else {
        init();

        net::dns::resolve(host, port, [ptr = context()](int err, const net::Address& addr) {
            if (!ptr->deleted) {
                auto handle = reinterpret_cast<TCPSocket*>(ptr->handle);
                if (err)
                    handle->setUVError(err, "DNS failed to resolve");
                else
                    handle->connect(addr);
            }
        }, loop());
    }
}


void TCPSocket::bind(const net::Address& address, unsigned flags)
{
    // LTrace("Binding on", address)

    // Reset the handle if the address family has changed
    if (_af != address.af()) {
        _af = address.af();
        reset();
    }

    if (_af == AF_INET6)
        flags |= UV_TCP_IPV6ONLY;

    invoke(&uv_tcp_bind, get(), address.addr(), flags); // "TCP bind failed"
}


void TCPSocket::listen(int backlog)
{
    // LTrace("Listening")
    init();

    invoke(&uv_listen, get<uv_stream_t>(), backlog,
        [](uv_stream_t* handle, int status) {
            auto self = reinterpret_cast<TCPSocket*>(handle->data);
            if (status == 0) {
                self->acceptConnection();
            }
            else {
                LError("Accept connection failed:", uv_strerror(status));
            }
        }); // "TCP listen failed"
}


bool TCPSocket::shutdown()
{
    // LTrace("Shutdown")
    return Stream::shutdown();
}


void TCPSocket::close()
{
    // LTrace("Close")
    Stream::close();
}


bool TCPSocket::setNoDelay(bool enable)
{
    assert(initialized());
    return uv_tcp_nodelay(get(), enable ? 1 : 0) == 0;
}


bool TCPSocket::setKeepAlive(bool enable, int delay)
{
    assert(initialized());
    return uv_tcp_keepalive(get(), enable ? 1 : 0, delay) == 0;
}


bool TCPSocket::setSimultaneousAccepts(bool enable)
{
    assert(initialized());
#ifdef SCY_WIN
    return uv_tcp_simultaneous_accepts(get(), enable ? 1 : 0) == 0;
#else
    return false;
#endif
}


bool TCPSocket::setReusePort()
{
    assert(initialized());
#if SCY_HAS_KERNEL_SOCKET_LOAD_BALANCING
    if (_af == AF_UNSPEC) {
        assert(0 && "bind() must be called first");
        return false;
    }

    uv_os_fd_t fd;
    uv_fileno(get<uv_handle_t>(), &fd);
    int on = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(int)) < 0) {
        LError("setsockopt(SO_REUSEPORT) failed")
        return false;
    }

    return true;
#else
    return false;
#endif
}


ssize_t TCPSocket::send(const char* data, size_t len, int flags)
{
    return send(data, len, peerAddress(), flags);
}


ssize_t TCPSocket::send(const char* data, size_t len, const net::Address& /* peerAddress */, int /* flags */)
{
    // LTrace("Send:", len, ":", std::string(data, len))
    assert(Thread::currentID() == tid());
    assert(initialized());

    // NOTE: libuv handles this for us
    // assert(len <= net::MAX_TCP_PACKET_SIZE);

    if (!Stream::write(data, len)) {
        LWarn("TCP send error")
        return -1;
    }

    // TODO: Return native error code
    return len;
}


net::Address TCPSocket::address() const
{
    if (initialized()) {
        struct sockaddr address;
        int addrlen = sizeof(address);
        if (uv_tcp_getsockname(get(), &address, &addrlen) == 0)
            return net::Address(&address, addrlen);
    }
    return net::Address();
}


net::Address TCPSocket::peerAddress() const
{
    if (initialized()) {
        struct sockaddr address;
        int addrlen = sizeof(address);
        if (uv_tcp_getpeername(get(), &address, &addrlen) == 0)
            return net::Address(&address, addrlen);
    }
    return net::Address();
}


void TCPSocket::setError(const scy::Error& err)
{
    assert(!error().any());
    Stream::setError(err);
}


const scy::Error& TCPSocket::error() const
{
    return Stream::error();
}


net::TransportType TCPSocket::transport() const
{
    return net::TCP;
}


bool TCPSocket::closed() const
{
    return Stream::closed();
}


uv::Loop* TCPSocket::loop() const
{
    return Stream::loop();
}


void TCPSocket::setMode(SocketMode mode)
{
    _mode = mode;
}


void* TCPSocket::self()
{
    return this;
}


const SocketMode TCPSocket::mode() const
{
    return _mode;
}


//
// Callbacks

void TCPSocket::onRead(const char* data, size_t len)
{
    // LTrace("On read:", len)

    // Note: The const_cast here is relatively safe since the given
    // data pointer is the underlying _buffer.data() pointer, but
    // a better way should be devised.
    onRecv(mutableBuffer(const_cast<char*>(data), len));
}


void TCPSocket::onRecv(const MutableBuffer& buf)
{
    // LTrace("On recv:", buf.size())
    onSocketRecv(*this, buf, peerAddress());
}


void TCPSocket::onConnect()
{
    // LTrace("On connect")

    if (readStart()) // will set error on failure
        onSocketConnect(*this);
}


void TCPSocket::acceptConnection()
{
    // Create the shared socket pointer so the if the socket handle is not
    // incremented the accepted socket will be destroyed.
    auto socket = net::makeSocket<net::TCPSocket>(loop());
    // LTrace("Accept connection:", socket->get())

    // invoke(&uv_tcp_init, loop(), socket->get()); // "Cannot initialize TCP socket"

    if (uv_accept(get<uv_stream_t>(), socket->get<uv_stream_t>()) == 0) {
        socket->readStart();
        AcceptConnection.emit(socket);
    }
    else {
        assert(0 && "uv_accept should not fail");
    }
}


// void TCPSocket::onAcceptConnection(uv_stream_t*, int status)
// {
//     if (status == 0) {
//         // LTrace("On accept connection")
//         acceptConnection();
//     }
//     else {
//         LError("Accept connection failed:", uv_strerror(status));
//     }
// }


void TCPSocket::onError(const scy::Error& error)
{
    // LDebug("Error:", error.message);
    onSocketError(*this, error);
    close(); // close on error
}


void TCPSocket::onClose()
{
    // LTrace("On close")
    onSocketClose(*this);
}


} // namespace net
} // namespace scy


/// @\}
