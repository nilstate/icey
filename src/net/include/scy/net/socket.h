///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#pragma once


#include "scy/base.h"
#include "scy/handle.h"
#include "scy/net/address.h"
#include "scy/net/net.h"
#include "scy/net/socketadapter.h"
#include "scy/packetstream.h"

#include "uv.h"

#include <any>
#include <stdexcept>


namespace scy {
namespace net {


/// Helper method for instantiating Sockets wrapped in a `std::shared_ptr`
/// that will be garbage collected on destruction.
template <class SocketT>
inline std::shared_ptr<SocketT> makeSocket(uv::Loop* loop = uv::defaultLoop())
{
    // std::shared_ptr<SocketT>(new SocketT(loop), deleter::Deferred<SocketT>());
    return std::make_shared<SocketT>(loop);
}


/// Base socket implementation from which all sockets derive.
class Net_API Socket : public SocketAdapter
{
public:
    using Ptr = std::shared_ptr<Socket>;
    using Vec = std::vector<Ptr>;

    Socket() = default;
    virtual ~Socket() noexcept = default;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&&) = delete;
    Socket& operator=(Socket&&) = delete;

    /// Connects to the given peer IP address.
    ///
    /// Throws an exception if the address is malformed.
    /// Connection errors can be handled via the Error signal.
    virtual void connect(const Address& address) = 0;

    /// Resolves and connects to the given host address.
    ///
    /// Throws an Exception if the host is malformed.
    /// Since the DNS callback is asynchronous implementations need
    /// to listen for the Error signal for handling connection errors.
    virtual void connect(const std::string& host, uint16_t port) = 0;

    /// Bind a local address to the socket.
    /// The address may be IPv4 or IPv6 (if supported).
    ///
    /// Throws an Exception on error.
    virtual void bind(const Address& address, unsigned flags = 0) = 0;

    /// Listens the socket on the given address.
    ///
    /// Throws an Exception on error.
    virtual void listen(int backlog = 64) { (void)backlog; }

    /// Sends the shutdown packet which should result is socket
    /// closure via callback.
    [[nodiscard]] virtual bool shutdown()
    {
        throw std::logic_error("Socket::shutdown not implemented by protocol");
        return false;
    };

    /// Closes the underlying socket.
    virtual void close() = 0;

    /// The locally bound address.
    ///
    /// This function will not throw.
    /// A Wildcard 0.0.0.0:0 address is returned if
    /// the socket is closed or invalid.
    virtual Address address() const = 0;

    /// The connected peer address.
    ///
    /// This function will not throw.
    /// A Wildcard 0.0.0.0:0 address is returned if
    /// the socket is closed or invalid.
    virtual Address peerAddress() const = 0;

    /// The transport protocol: TCP, UDP or SSLTCP.
    virtual net::TransportType transport() const = 0;

    /// Sets the socket error.
    ///
    /// Setting the error will result in socket closure.
    virtual void setError(const scy::Error& err) = 0;

    /// Return the socket error if any.
    virtual const scy::Error& error() const = 0;

    /// Returns true if the native socket handle is closed.
    virtual bool closed() const = 0;

    /// Returns the socket event loop.
    virtual uv::Loop* loop() const = 0;

    /// Optional client data.
    ///
    /// The value is empty on initialization.
    std::any opaque;

protected:
    /// Initializes the underlying socket context.
    virtual void init() = 0;

    /// Resets the socket context for reuse.
    virtual void reset() = 0;

    int _af{AF_UNSPEC};
};


//
// Packet Info
//


/// Provides information about packets emitted from a socket.
/// See SocketPacket.
struct PacketInfo : public IPacketInfo
{
    /// The source socket
    Socket::Ptr socket;

    /// The originating peer address.
    /// For TCP this will always be connected address.
    Address peerAddress;

    PacketInfo(const Socket::Ptr& socket, const Address& peerAddress)
        : socket(socket)
        , peerAddress(peerAddress)
    {
    }

    PacketInfo(const PacketInfo& r)
        : socket(r.socket)
        , peerAddress(r.peerAddress)
    {
    }

    virtual std::unique_ptr<IPacketInfo> clone() const { return std::make_unique<PacketInfo>(*this); }

    virtual ~PacketInfo() noexcept = default;
};


//
// Socket Packet
//


/// SocketPacket is the default packet type emitted by sockets.
/// SocketPacket provides peer address information and a buffer
/// reference for nocopy binary operations.
///
/// The referenced packet buffer lifetime is only guaranteed
/// for the duration of the receiver callback.
class Net_API SocketPacket : public RawPacket
{
public:
    SocketPacket(const Socket::Ptr& socket, const MutableBuffer& buffer, const Address& peerAddress)
        : RawPacket(bufferCast<char*>(buffer), buffer.size(), 0, nullptr,
                    std::make_unique<PacketInfo>(socket, peerAddress))
    {
    }

    SocketPacket(const SocketPacket& that)
        : RawPacket(that)
    {
    }

    /// Returns the PacketInfo for this socket packet.
    PacketInfo* packetInfo() const
    {
        return static_cast<PacketInfo*>(info.get());
    }

    virtual ~SocketPacket() noexcept = default;

    virtual void print(std::ostream& os) const
    {
        os << className() << ": " << packetInfo()->peerAddress << '\n';
    }

    virtual std::unique_ptr<IPacket> clone() const { return std::make_unique<SocketPacket>(*this); }

    virtual ssize_t read(const ConstBuffer&)
    {
        throw std::logic_error("SocketPacket::read not supported; write only");
        return 0;
    }

    virtual void write(Buffer& buf) const
    {
        // buf.append(data(), size());
        buf.insert(buf.end(), data(), data() + size());
    }

    virtual const char* className() const { return "SocketPacket"; }
};


} // namespace net
} // namespace scy


/// @\}
