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
#include "icy/net/tcpsocket.h"
#include "icy/stream.h"


namespace icy {
namespace net {


/// TCP socket implementation.
class Net_API TCPSocket : public Stream<uv_tcp_t>
    , public net::Socket
{
public:
    using Ptr = std::shared_ptr<TCPSocket>;
    using Vec = std::vector<Ptr>;

    /// Constructs the TCPSocket and initializes the underlying libuv handle.
    /// @param loop Event loop to use; defaults to the default loop.
    TCPSocket(uv::Loop* loop = uv::defaultLoop());
    virtual ~TCPSocket() noexcept;

    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
    TCPSocket(TCPSocket&&) = delete;
    TCPSocket& operator=(TCPSocket&&) = delete;

    /// Sends a TCP shutdown request; the socket closes after the peer acknowledges.
    /// @return true if the shutdown request was queued successfully.
    [[nodiscard]] bool shutdown() override;

    /// Closes the socket immediately, releasing all associated resources.
    virtual void close() override;

    /// Connects to @p peerAddress using a libuv connect request.
    /// On success, calls onConnect(); on failure, calls setUVError().
    /// @param peerAddress The remote address to connect to.
    virtual void connect(const net::Address& peerAddress) override;

    /// Resolves @p host via DNS (or maps "localhost"), then connects.
    /// @param host Hostname or IP address string.
    /// @param port Destination port.
    virtual void connect(const std::string& host, uint16_t port) override;

    /// Writes @p len bytes to the connected peer.
    /// @param data  Pointer to the data to send.
    /// @param len   Number of bytes to send.
    /// @param flags Reserved; currently unused.
    /// @return Number of bytes sent, or -1 on error.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, int flags = 0) override;
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, int flags = 0) override;

    /// Writes @p len bytes; @p peerAddress is ignored for TCP (connected stream).
    /// @param data        Pointer to the data to send.
    /// @param len         Number of bytes to send.
    /// @param peerAddress Ignored; present for interface conformance.
    /// @param flags       Reserved; currently unused.
    /// @return Number of bytes sent, or -1 on error.
    [[nodiscard]] virtual ssize_t send(const char* data, size_t len, const net::Address& peerAddress, int flags = 0) override;
    [[nodiscard]] virtual ssize_t sendOwned(Buffer&& buffer, const net::Address& peerAddress, int flags = 0) override;

    /// Binds the socket to @p address.
    /// Resets and reinitializes the handle if the address family changes.
    /// @param address Local address to bind to.
    /// @param flags   Optional bind flags (e.g. UV_TCP_IPV6ONLY is added automatically for IPv6).
    virtual void bind(const net::Address& address, unsigned flags = 0) override;

    /// Starts listening for incoming connections with the given backlog.
    /// @param backlog Maximum length of the pending connection queue.
    virtual void listen(int backlog = 64) override;

    /// Accepts the next pending client connection and fires AcceptConnection.
    virtual void acceptConnection();

    /// Enables SO_REUSEPORT on Linux kernel >= 3.9 for multi-thread load balancing.
    /// Must be called after bind(). No-op and returns false on unsupported platforms.
    /// @return true if the socket option was set successfully.
    [[nodiscard]] bool setReusePort();

    /// Enables or disables TCP_NODELAY (Nagle's algorithm).
    /// @param enable true to disable Nagle's algorithm (lower latency).
    /// @return true if the option was set successfully.
    [[nodiscard]] bool setNoDelay(bool enable);

    /// Enables or disables TCP keep-alive probes.
    /// @param enable true to enable keep-alive.
    /// @param delay  Initial delay in seconds before the first keep-alive probe.
    /// @return true if the option was set successfully.
    [[nodiscard]] bool setKeepAlive(bool enable, int delay);

    /// Enables or disables simultaneous accepts on Windows.
    /// No-op and returns false on non-Windows platforms.
    /// @param enable true to enable simultaneous accepts.
    /// @return true if the option was set successfully.
    [[nodiscard]] bool setSimultaneousAccepts(bool enable);

    /// Sets the socket mode (ServerSide or ClientSide).
    /// @param mode The mode to assign.
    void setMode(SocketMode mode);

    /// Returns the current socket mode.
    SocketMode mode() const;

    /// Sets the socket error; ignores the call if an error is already recorded.
    /// Setting an error causes the socket to close.
    /// @param err The error to record.
    virtual void setError(const icy::Error& err) override;

    /// Returns the current socket error, if any.
    const icy::Error& error() const override;

    /// Returns true if the native socket handle is closed.
    virtual bool closed() const override;

    /// Returns the IP address and port number of the socket.
    /// A wildcard address is returned if the socket is not connected.
    net::Address address() const override;

    /// Returns the IP address and port number of the peer socket.
    /// A wildcard address is returned if the socket is not connected.
    net::Address peerAddress() const override;

    /// Returns the TCP transport protocol.
    net::TransportType transport() const override;

    /// Returns the event loop associated with this socket.
    virtual uv::Loop* loop() const override;

    /// Fired when a new client connection is accepted; carries a shared_ptr to the new socket.
    LocalSignal<void(const net::TCPSocket::Ptr&)> AcceptConnection;

public:
    /// Called by the stream layer when the TCP connection is established.
    virtual void onConnect();

    /// Called by the stream layer with raw received bytes; wraps them in a MutableBuffer.
    /// @param data Pointer to received bytes.
    /// @param len  Number of bytes received.
    virtual void onRead(const char* data, size_t len) override;

    /// Dispatches a received buffer to all socket adapters via onSocketRecv.
    /// @param buf The buffer containing the received data.
    virtual void onRecv(const MutableBuffer& buf);

    /// Dispatches the error to adapters and closes the socket.
    /// @param error The error that occurred.
    virtual void onError(const icy::Error& error) override;

    /// Dispatches the close event to all socket adapters.
    virtual void onClose() override;

protected:
    virtual void init() override;
    virtual void reset() override;

    SocketMode _mode;
    net::Address _peerAddress; ///< Cached peer address (avoids syscall per recv)
};


} // namespace net
} // namespace icy


/// @\}
