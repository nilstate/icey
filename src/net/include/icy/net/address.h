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


#include "icy/net/net.h"
#include <memory>
#include <string>
#include <string_view>


namespace icy {
namespace net {


/// Base class for network address implementations.
class Net_API AddressBase;
/// Represents an IPv4 or IPv6 socket address with host and port.
class Net_API Address
{
public:
    /// Possible address families for IP addresses.
    enum Family
    {
        IPv4,
        IPv6
    };

    /// Creates a wildcard (all zero) IPv4 Address.
    Address();

    /// Creates an Address from an IP address and a port number.
    ///
    /// The IP address must either be a domain name, or it must
    /// be in dotted decimal (IPv4) or hex string (IPv6) format.
    Address(const std::string& host, uint16_t port);

    /// Creates an Address by copying another one.
    Address(const Address& addr);

    /// Creates an Address from a native socket address.
    Address(const struct sockaddr* addr, socklen_t length);

    /// Creates an Address from an IP address and a
    /// service name or port number.
    ///
    /// The IP address must either be a domain name, or it must
    /// be in dotted decimal (IPv4) or hex string (IPv6) format.
    ///
    /// The given port must either be a decimal port number, or
    /// a service name.
    Address(const std::string& host, const std::string& port);

    /// Creates an Address from an IP address or host name and a
    /// port number/service name. Host name/address and port number must
    /// be separated by a colon. In case of an IPv6 address,
    /// the address part must be enclosed in brackets.
    ///
    /// Examples:
    ///     192.168.1.10:80
    ///     [::ffff:192.168.1.120]:2040
    ///     www.0state.com:8080
    explicit Address(const std::string& hostAndPort);

    /// Destroys the Address.
    ~Address() noexcept;

    /// Assigns another Address.
    Address& operator=(const Address& addr);

    /// Swaps the Address with another one.
    void swap(Address& addr);

    /// Returns the host IP address.
    std::string host() const;

    /// Returns the port number.
    uint16_t port() const;

    /// Returns the length of the internal native socket address.
    socklen_t length() const;

    /// Returns a pointer to the internal native socket address.
    const struct sockaddr* addr() const;

    /// Returns the address family (AF_INET or AF_INET6) of the address.
    int af() const;

    /// Returns a string representation of the address.
    std::string toString() const;

    /// Returns the address family of the host's address.
    Address::Family family() const;

    /// Returns true when the port is set and the address is valid
    /// ie. not wildcard.
    bool valid() const;

    /// Resolves a service name or decimal port string to a port number.
    /// @param service Service name (e.g. "http") or decimal port string (e.g. "80").
    /// @return The resolved port number in host byte order.
    static uint16_t resolveService(const std::string& service);

    /// Returns true if the given string is a valid IPv4 or IPv6 address.
    /// @param address The string to validate.
    /// @return true if the address parses as a valid IP address, false otherwise.
    static bool validateIP(std::string_view address);

    /// Compares two addresses for ordering (by family then port).
    /// @param addr The address to compare against.
    /// @return true if this address is less than @p addr.
    bool operator<(const Address& addr) const;

    /// Returns true if the host and port of both addresses are equal.
    /// @param addr The address to compare against.
    bool operator==(const Address& addr) const;

    /// Returns true if the host or port of the addresses differ.
    /// @param addr The address to compare against.
    bool operator!=(const Address& addr) const;

    friend std::ostream& operator<<(std::ostream& stream, const Address& addr)
    {
        stream << addr.toString();
        return stream;
    }

protected:
    void init(const std::string& host, uint16_t port);

private:
    std::shared_ptr<AddressBase> _base;
};


} // namespace net
} // namespace icy


/// @}
