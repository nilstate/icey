///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "icy/handle.h"
#include "icy/net/address.h"
#include "icy/net/net.h"
#include "icy/net/socket.h"


namespace icy {
namespace net {


/// UDP socket implementation.
class Net_API UDPSocket : public uv::Handle<uv_udp_t>
    , public net::Socket
{
public:
    using Ptr = std::shared_ptr<UDPSocket>;
    using Vec = std::vector<Ptr>;

    /// Constructs the UDPSocket and initializes the underlying libuv handle.
    /// @param loop Event loop to use; defaults to the default loop.
    UDPSocket(uv::Loop* loop = uv::defaultLoop());
    virtual ~UDPSocket() noexcept;

    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;
    UDPSocket(UDPSocket&&) = delete;
    UDPSocket& operator=(UDPSocket&&) = delete;

    /// Records the peer address and fires the Connect signal to mimic TCP socket behaviour.
    /// UDP is connectionless; this call does not send any data.
    /// @param peerAddress The remote address to associate with this socket.
    virtual void connect(const net::Address& peerAddress) override;

    /// Resolves @p host via DNS (or maps "localhost"), then calls connect(Address).
    /// @param host Hostname or IP address string.
    /// @param port Destination port.
    virtual void connect(const std::string& host, uint16_t port) override;

    /// Stops receiving and closes the underlying UDP handle.
    virtual void close() override;

    /// Binds the socket to @p address and starts the receive loop.
    /// @param address Local address to bind to.
    /// @param flags   Optional bind flags (UV_UDP_IPV6ONLY is added automatically for IPv6).
    virtual void bind(const net::Address& address, unsigned flags = 0) override;

    /// Sends @p len bytes to the previously connected peer address.
    /// Returns -1 if no peer address has been set.
    /// @param data  Pointer to the data to send.
    /// @param len   Number of bytes to send.
    /// @param flags Reserved; currently unused.
    /// @return Number of bytes accepted for sending, or -1 on error.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, int flags = 0) override;

    /// Sends @p len bytes to @p peerAddress.
    /// Returns -1 if the socket is uninitialized or the address is not authorized.
    /// @param data        Pointer to the data to send.
    /// @param len         Number of bytes to send.
    /// @param peerAddress Destination address; must match the connected peer if one is set.
    /// @param flags       Reserved; currently unused.
    /// @return Number of bytes accepted for sending, or -1 on error.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len,
                                       const net::Address& peerAddress, int flags = 0) override;

    /// Enables or disables UDP broadcast.
    /// @param flag true to enable broadcast.
    /// @return true if the option was set successfully.
    [[nodiscard]] bool setBroadcast(bool flag);

    /// Enables or disables IP multicast loopback.
    /// @param flag true to enable multicast loopback.
    /// @return true if the option was set successfully.
    [[nodiscard]] bool setMulticastLoop(bool flag);

    /// Sets the IP multicast time-to-live (hop limit).
    /// @param ttl Value in the range [1, 255].
    /// @return true if the option was set successfully.
    /// @throws std::invalid_argument if @p ttl is out of range.
    [[nodiscard]] bool setMulticastTTL(int ttl);

    /// Returns the locally bound address, or a wildcard address if unbound.
    virtual net::Address address() const override;

    /// Returns the connected peer address set by connect(), or a wildcard address if unconnected.
    virtual net::Address peerAddress() const override;

    /// Returns the UDP transport protocol.
    net::TransportType transport() const override;

    /// Sets the socket error and triggers close.
    /// @param err The error to record.
    virtual void setError(const icy::Error& err) override;

    /// Returns the current socket error, if any.
    virtual const icy::Error& error() const override;

    /// Returns true if the native socket handle is closed.
    virtual bool closed() const override;

    /// Returns the event loop associated with this socket.
    virtual uv::Loop* loop() const override;

    /// Dispatches a received datagram to all socket adapters via onSocketRecv.
    /// @param buf     Buffer containing the received datagram payload.
    /// @param address Address of the sender.
    virtual void onRecv(const MutableBuffer& buf, const net::Address& address);

protected:
    virtual void init() override;
    virtual void reset() override;

    virtual void onError(const icy::Error& error) override;
    virtual void onClose() override;

    virtual bool recvStart();
    virtual bool recvStop();

    static void onRecv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
                       const struct sockaddr* addr, unsigned flags);
    static void allocRecvBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

    net::Address _peer;
    Buffer _buffer;
};


} // namespace net
} // namespace icy


/// @\}
