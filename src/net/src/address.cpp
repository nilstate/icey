///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup net
/// @{


#include "icy/net/address.h"
#include "icy/logger.h"
#include "icy/util.h"
#include <cstdint>




namespace icy {
namespace net {


//
// AddressBase
//


/// Base class for network address implementations
class Net_API AddressBase
{
public:
    virtual std::string host() const = 0;
    virtual uint16_t port() const = 0;
    virtual Address::Family family() const = 0;
    virtual socklen_t length() const = 0;
    virtual const struct sockaddr* addr() const = 0;
    virtual int af() const = 0;

protected:
    AddressBase() {}
    virtual ~AddressBase() {}

private:
    AddressBase(const AddressBase&) = delete;
    AddressBase& operator=(const AddressBase&) = delete;
};


//
// IPv4AddressBase
//


/// IPv4-specific network address implementation
class Net_API IPv4AddressBase : public AddressBase
{
public:
    IPv4AddressBase()
    {
        _addr.sin_family = AF_INET;
        memset(&_addr, 0, sizeof(_addr));
    }

    IPv4AddressBase(const struct sockaddr_in* addr)
    {
        _addr.sin_family = AF_INET;
        memcpy(&_addr, addr, sizeof(_addr));
    }

    IPv4AddressBase(const void* addr, uint16_t port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        memcpy(&_addr.sin_addr, addr, sizeof(_addr.sin_addr));
        _addr.sin_port = port;
    }

    std::string host() const
    {
        char dest[16];
        if (uv_ip4_name(const_cast<sockaddr_in*>(&_addr), dest, 16) != 0)
            throw std::runtime_error("Cannot parse IPv4 hostname");
        return dest;
    }

    uint16_t port() const { return _addr.sin_port; }

    Address::Family family() const { return Address::IPv4; }

    socklen_t length() const { return sizeof(_addr); }

    const struct sockaddr* addr() const
    {
        return reinterpret_cast<const struct sockaddr*>(&_addr);
    }

    int af() const { return _addr.sin_family; }

private:
    struct sockaddr_in _addr;
};


//
// IPv6AddressBase
//

#if defined(ICY_HAVE_IPv6)


class Net_API IPv6AddressBase : public AddressBase
{
public:
    IPv6AddressBase(const struct sockaddr_in6* addr)
    {
        _addr.sin6_family = AF_INET6;
        memcpy(&_addr, addr, sizeof(_addr));
    }

    IPv6AddressBase(const void* addr, uint16_t port)
    {
        _addr.sin6_family = AF_INET6;
        memset(&_addr, 0, sizeof(_addr));
        memcpy(&_addr.sin6_addr, addr, sizeof(_addr.sin6_addr));
        _addr.sin6_port = port;
    }

    IPv6AddressBase(const void* addr, uint16_t port, uint32_t scope)
    {
        _addr.sin6_family = AF_INET6;
        memset(&_addr, 0, sizeof(_addr));
        memcpy(&_addr.sin6_addr, addr, sizeof(_addr.sin6_addr));
        _addr.sin6_port = port;
        _addr.sin6_scope_id = scope;
    }

    std::string host() const
    {
        char dest[46];
        if (uv_ip6_name(const_cast<sockaddr_in6*>(&_addr), dest, 46) != 0)
            throw std::runtime_error("Cannot parse IPv6 hostname");
        return dest;
    }

    uint16_t port() const { return _addr.sin6_port; }

    Address::Family family() const { return Address::IPv6; }

    socklen_t length() const { return sizeof(_addr); }

    const struct sockaddr* addr() const
    {
        return reinterpret_cast<const struct sockaddr*>(&_addr);
    }

    int af() const { return _addr.sin6_family; }

private:
    struct sockaddr_in6 _addr;
};


#endif // ICY_HAVE_IPv6


//
// Address
//


Address::Address()
    : _base(std::make_shared<IPv4AddressBase>())
{
}


Address::Address(const std::string& addr, uint16_t port)
{
    init(addr, port);
}


Address::Address(const std::string& addr, const std::string& port)
{
    init(addr, resolveService(port));
}


Address::Address(const std::string& hostAndPort)
{
    if (hostAndPort.empty())
        throw std::invalid_argument("Address: empty host:port string");

    std::string host;
    std::string port;
    std::string::const_iterator it = hostAndPort.begin();
    std::string::const_iterator end = hostAndPort.end();
    if (*it == '[') {
        ++it;
        while (it != end && *it != ']')
            host += *it++;
        if (it == end)
            throw std::runtime_error("Invalid address: Malformed IPv6 address");
        ++it;
    } else {
        while (it != end && *it != ':')
            host += *it++;
    }
    if (it != end && *it == ':') {
        ++it;
        while (it != end)
            port += *it++;
    } else
        throw std::runtime_error("Invalid address: Missing port number");
    init(host, resolveService(port));
}


Address::Address(const struct sockaddr* addr, socklen_t length)
{
    if (length == sizeof(struct sockaddr_in))
        _base = std::make_shared<IPv4AddressBase>(
            reinterpret_cast<const struct sockaddr_in*>(addr));
#if defined(ICY_HAVE_IPv6)
    else if (length == sizeof(struct sockaddr_in6))
        _base = std::make_shared<IPv6AddressBase>(
            reinterpret_cast<const struct sockaddr_in6*>(addr));
#endif
    else
        throw std::runtime_error("Invalid address length passed to Address()");
}


Address::Address(const Address& addr)
{
    _base = addr._base;
}


Address::~Address() noexcept
{
}


Address& Address::operator=(const Address& addr)
{
    if (&addr != this) {
        _base = addr._base;
    }
    return *this;
}


void Address::init(const std::string& host, uint16_t port)
{
    char ia[sizeof(struct in6_addr)];
    if (uv_inet_pton(AF_INET, host.c_str(), &ia) == 0)
        _base = std::make_shared<IPv4AddressBase>(&ia, htons(port));
    else if (uv_inet_pton(AF_INET6, host.c_str(), &ia) == 0)
        _base = std::make_shared<IPv6AddressBase>(&ia, htons(port));
    else
        throw std::runtime_error("Invalid IP address format: " + host);
}


std::string Address::host() const
{
    return _base->host();
}


uint16_t Address::port() const
{
    return ntohs(_base->port());
}


Address::Family Address::family() const
{
    return _base->family();
}


socklen_t Address::length() const
{
    return _base->length();
}


const struct sockaddr* Address::addr() const
{
    return _base->addr();
}


int Address::af() const
{
    return _base->af();
}


bool Address::valid() const
{
    return host() != "0.0.0.0" && port() != 0;
}


std::string Address::toString() const
{
    std::string result;
    if (family() == Address::IPv6)
        result.append("[");
    result.append(host());
    if (family() == Address::IPv6)
        result.append("]");
    result.append(":");
    result.append(util::itostr(port()));
    return result;
}


bool Address::operator<(const Address& addr) const
{
    if (family() != addr.family())
        return family() < addr.family();
    if (host() != addr.host())
        return host() < addr.host();
    return port() < addr.port();
}


bool Address::operator==(const Address& addr) const
{
    return host() == addr.host() && port() == addr.port();
}


bool Address::operator!=(const Address& addr) const
{
    return host() != addr.host() || port() != addr.port();
}


// void Address::swap(Address& a1, Address& a2)
// {
//     a1.swap(a2);
// }


void Address::swap(Address& addr)
{
    std::swap(_base, addr._base);
}


//
// Static helpers
//


bool Address::validateIP(std::string_view addr)
{
    std::string owned(addr);
    char ia[sizeof(struct in6_addr)];
    if (uv_inet_pton(AF_INET, owned.c_str(), &ia) == 0)
        return true;
    else if (uv_inet_pton(AF_INET6, owned.c_str(), &ia) == 0)
        return true;
    return false;
}


uint16_t Address::resolveService(const std::string& service)
{
    auto port = util::strtoi<uint16_t>(service);
    if (port && port > 0) //, port) && port <= 0xFFFF
        return port;

    struct servent* serv = getservbyname(service.c_str(), nullptr);
    if (serv)
        return ntohs(serv->s_port);
    else
        throw std::runtime_error("Service not found: " + service);
}


} // namespace net
} // namespace icy


/// @}
