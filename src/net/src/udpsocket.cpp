///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "icy/net/udpsocket.h"
#include "icy/logger.h"
#include "icy/net/net.h"

#include <memory>


using namespace std;


namespace icy {
namespace net {

namespace {

struct OwnedUDPSendReq
{
    uv_udp_send_t req{};
    Buffer buffer;
};

} // namespace


UDPSocket::UDPSocket(uv::Loop* loop)
    : uv::Handle<uv_udp_t>(loop)
    , _buffer(65536)
{
    // LTrace("Create");
    init();
}


UDPSocket::~UDPSocket() noexcept
{
    // LTrace("Destroy");
    close();
}


void UDPSocket::init()
{
    if (initialized())
        return;

    // LTrace("Init");

    if (!get())
        uv::Handle<uv_udp_t>::reset();
    uv::Handle<uv_udp_t>::init(&uv_udp_init);
    get()->data = this;
}


void UDPSocket::reset()
{
    // LTrace("Reset");
    uv::Handle<uv_udp_t>::reset();
    init();
    get()->data = this;
}


void UDPSocket::connect(const Address& peerAddress)
{
    // LTrace("Connect:", peerAddress);
    init();
    _peer = peerAddress;

    // Emit the Connected signal to mimic TCPSocket behaviour
    // since socket implementations are interchangable.
    onSocketConnect(*this);
}


void UDPSocket::connect(const std::string& host, uint16_t port)
{
    if (Address::validateIP(host)) {
        connect(Address(host, port));
    } else if (host == "localhost") {
        // Resolve localhost directly to loopback: some systems' getaddrinfo
        // returns 0.0.0.0 for "localhost" which causes connection failure.
        connect(Address("127.0.0.1", port));
    } else {
        init();

        net::dns::resolve(host, port, uv::withHandleContext(*this,
            [](UDPSocket& handle, int err, const net::Address& addr) {
                if (err)
                    handle.setUVError(err, "DNS failed to resolve");
                else
                    handle.connect(addr);
            }), loop());
    }
}


void UDPSocket::close()
{
    // LTrace("Closing");
    if (initialized() && !closed())
        recvStop();
    uv::Handle<uv_udp_t>::close();
}


void UDPSocket::bind(const Address& address, unsigned flags)
{
    // LTrace("Binding on", address);
    init();

    if (address.af() == AF_INET6)
        flags |= UV_UDP_IPV6ONLY;

    if (invoke(&uv_udp_bind, get(), address.addr(), flags))
        recvStart();
}


ssize_t UDPSocket::send(const char* data, size_t len, int flags)
{
    if (!_peer.valid())
        return -1;
    return send(data, len, _peer, flags);
}


ssize_t UDPSocket::sendOwned(Buffer&& buffer, int flags)
{
    if (!_peer.valid())
        return -1;
    return sendOwned(std::move(buffer), _peer, flags);
}


ssize_t UDPSocket::send(const char* data, size_t len, const Address& peerAddress, int /* flags */)
{
    // LTrace("Send:", len, ":", peerAddress);
    if (!initialized())
        return -1;
    // assert(len <= net::MAX_UDP_PACKET_SIZE);

    if (_peer.valid() && _peer != peerAddress) {
        LError("Peer not authorized:", peerAddress);
        return -1;
    }

    if (!peerAddress.valid()) {
        LError("Peer not valid:", peerAddress);
        return -1;
    }

    auto buf = uv_buf_init(const_cast<char*>(data), static_cast<unsigned int>(len));
    auto* req = new uv_udp_send_t;
    int err = uv_udp_send(req, get(), &buf, 1, peerAddress.addr(),
                          [](uv_udp_send_t* req, int) {
                              delete req;
                          });
    if (!err) {
        return static_cast<ssize_t>(len);
    }
    delete req;
    setUVError(err, "UV Error");
    return error().err;
}


ssize_t UDPSocket::sendOwned(Buffer&& buffer, const Address& peerAddress, int /* flags */)
{
    if (!initialized())
        return -1;

    if (_peer.valid() && _peer != peerAddress) {
        LError("Peer not authorized:", peerAddress);
        return -1;
    }

    if (!peerAddress.valid()) {
        LError("Peer not valid:", peerAddress);
        return -1;
    }

    auto* req = new OwnedUDPSendReq;
    req->buffer = std::move(buffer);
    auto len = req->buffer.size();
    auto uvBuffer = uv_buf_init(req->buffer.data(), static_cast<unsigned int>(req->buffer.size()));
    int err = uv_udp_send(&req->req, get(), &uvBuffer, 1, peerAddress.addr(),
                          [](uv_udp_send_t* req, int) {
                              delete reinterpret_cast<OwnedUDPSendReq*>(req);
                          });
    if (err) {
        delete req;
        setUVError(err, "UV Error");
        return error().err;
    }

    return static_cast<ssize_t>(len);
}


bool UDPSocket::setBroadcast(bool enable)
{
    if (!initialized())
        return false;
    return uv_udp_set_broadcast(get(), enable ? 1 : 0) == 0;
}


bool UDPSocket::setMulticastLoop(bool enable)
{
    if (!initialized())
        return false;
    return uv_udp_set_multicast_loop(get(), enable ? 1 : 0) == 0;
}


bool UDPSocket::setMulticastTTL(int ttl)
{
    if (!initialized())
        return false;
    if (ttl <= 0 || ttl > 255)
        throw std::invalid_argument("TTL out of range");
    return uv_udp_set_multicast_ttl(get(), ttl) == 0;
}


bool UDPSocket::recvStart()
{
    if (!initialized() || closed())
        return false;

    // LTrace("Recv start");

    // UV_EALREADY means that the socket is already bound but that's okay.
    int r = uv_udp_recv_start(get(), UDPSocket::allocRecvBuffer, UDPSocket::onRecv);
    if (r) { // && r != UV_EALREADY
        setUVError(r, "Cannot start recv on invalid UDP socket");
        return false;
    }
    return true;
}


bool UDPSocket::recvStop()
{
    if (!initialized() || closed())
        return false;

    // This method must not throw since it is called
    // internally via libuv callbacks.
    return get() && uv_udp_recv_stop(get()) == 0;
}


void UDPSocket::onRecv(const MutableBuffer& buf, const net::Address& address)
{
    // LTrace("On recv:", buf.size(), ":", address);
    onSocketRecv(*this, buf, address);
}


void UDPSocket::setError(const icy::Error& err)
{
    uv::Handle<uv_udp_t>::setError(err);
}


const icy::Error& UDPSocket::error() const
{
    return uv::Handle<uv_udp_t>::error();
}


net::Address UDPSocket::address() const
{
    if (initialized()) {
        struct sockaddr address;
        int addrlen = sizeof(address);
        if (uv_udp_getsockname(get(), &address, &addrlen) == 0)
            return net::Address(&address, addrlen);
    }
    return net::Address();
}


net::Address UDPSocket::peerAddress() const
{
    return _peer;
}


net::TransportType UDPSocket::transport() const
{
    return net::UDP;
}


bool UDPSocket::closed() const
{
    return uv::Handle<uv_udp_t>::closed();
}


//
// Callbacks

void UDPSocket::onRecv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
                       const struct sockaddr* addr, unsigned /* flags */)
{
    auto socket = reinterpret_cast<UDPSocket*>(handle->data);
    if (!socket->initialized() || socket->closed()) {
        LWarn("Recv on uninitialized or closed socket");
        return;
    }
    // LTrace("On read:", nread);

    if (nread < 0) {
        // assert(0 && "unexpected error");
        LDebug("Recv error:", uv_err_name(static_cast<int>(nread)));
        socket->setUVError(static_cast<int>(nread), "UDP recv error");
        return;
    } else if (nread == 0) {
        // Returning unused buffer, this is not an error
        // 11/12/13: This happens on linux but not windows
        // socket->setUVError(UV_EOF, "End of file");
        return;
    }

    socket->onRecv(mutableBuffer(buf->base, nread),
                   net::Address(addr, sizeof(*addr)));
}


void UDPSocket::allocRecvBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    auto& buffer = static_cast<UDPSocket*>(handle->data)->_buffer;

    // Resize to libuv's suggested size (typically 65536 for UDP)
    if (buffer.size() < suggested_size)
        buffer.resize(suggested_size);

    buf->base = buffer.data();
    buf->len = buffer.size();
}


void UDPSocket::onError(const icy::Error& error)
{
    // LDebug("Error", error.message);
    onSocketError(*this, error);
    close(); // close on error
}


void UDPSocket::onClose()
{
    // LDebug("On close");
    onSocketClose(*this);
}


uv::Loop* UDPSocket::loop() const
{
    return uv::Handle<uv_udp_t>::loop();
}


} // namespace net
} // namespace icy


/// @\}
