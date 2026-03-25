///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#ifdef ICY_WIN
#include <winsock2.h>
#endif

#include "icy/crypto/hmac.h"
#include "icy/logger.h"
#include "icy/stun/attributes.h"
#include "icy/stun/message.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>


namespace icy {
namespace stun {

namespace {

bool isAddressAttributeSize(uint16_t size)
{
    return size == AddressAttribute::IPv4Size || size == AddressAttribute::IPv6Size;
}

template <typename T>
std::unique_ptr<Attribute> createFixedAttribute(uint16_t size)
{
    return size == T::Size ? std::make_unique<T>() : nullptr;
}

template <typename T>
std::unique_ptr<Attribute> createBoundedAttribute(uint16_t size, uint16_t maxSize)
{
    return size <= maxSize ? std::make_unique<T>(size) : nullptr;
}

template <typename T>
std::unique_ptr<Attribute> createExactSizeAttribute(uint16_t size, uint16_t expectedSize)
{
    return size == expectedSize ? std::make_unique<T>(size) : nullptr;
}

template <typename T>
std::unique_ptr<Attribute> createVariableAttribute(uint16_t size)
{
    return std::make_unique<T>(size);
}

} // namespace


Attribute::Attribute(uint16_t type, uint16_t size)
    : _type(type)
    , _size(size)
{
}


std::string Attribute::typeString(uint16_t type)
{
    switch (type) {
        case Attribute::XorMappedAddress:
            return "XOR-MAPPED-ADDRESS";
        case Attribute::XorPeerAddress:
            return "XOR-PEER-ADDRESS";
        case Attribute::XorRelayedAddress:
            return "XOR-RELAYED-ADDRESS";
        case Attribute::MappedAddress:
            return "MAPPED-ADDRESS";
        case Attribute::ResponseAddress:
            return "RESPONSE-ADDRESS";
        case Attribute::ChangeRequest:
            return "CHANGE-REQUEST";
        case Attribute::SourceAddress:
            return "SOURCE-ADDRESS";
        case Attribute::ChangedAddress:
            return "CHANGED-ADDRESS";
        case Attribute::Username:
            return "USERNAME";
        case Attribute::Password:
            return "PASSWORD";
        case Attribute::MessageIntegrity:
            return "MESSAGE-INTEGRITY";
        case Attribute::ErrorCode:
            return "ERROR-CODE";
        case Attribute::Bandwidth:
            return "BANDWIDTH";
        case Attribute::DestinationAddress:
            return "DESTINATION-ADDRESS";
        case Attribute::UnknownAttributes:
            return "UNKNOWN-ATTRIBUTES";
        case Attribute::ReflectedFrom:
            return "REFLECTED-FROM";
        // case Attribute::TransportPreferences: return "TRANSPORT-PREFERENCES";
        case Attribute::MagicCookie:
            return "MAGIC-COOKIE";
        case Attribute::Realm:
            return "REALM";
        case Attribute::Nonce:
            return "NONCE";
        case Attribute::Software:
            return "SOFTWARE";
        case Attribute::Options:
            return "OPTIONS";
        case Attribute::AlternateServer:
            return "ALTERNATE-SERVER";
        case Attribute::Fingerprint:
            return "FINGERPRINT";
        case Attribute::ChannelNumber:
            return "CHANNEL-NUMBER";
        case Attribute::Lifetime:
            return "LIFETIME";
        case Attribute::Data:
            return "DATA";
        case Attribute::RequestedTransport:
            return "REQUESTED-TRANSPORT";
        case Attribute::ReservationToken:
            return "RESERVATION-TOKEN";
        case Attribute::EventPort:
            return "EVEN-PORT";
        case Attribute::DontFragment:
            return "DONT-FRAGMENT";
        case Attribute::ICEControlled:
            return "ICE-CONTROLLED";
        case Attribute::ICEControlling:
            return "ICE-CONTROLLING";
        case Attribute::ICEPriority:
            return "PRIORITY";
        case Attribute::ICEUseCandidate:
            return "USE-CANDIDATE";
        case Attribute::ConnectionID:
            return "CONNECTION-ID";
        default:
            return "Unknown";
    }
}


uint16_t Attribute::size() const
{
    return _size;
}


uint16_t Attribute::type() const // Attribute::Type
{
    return _type; // static_cast<Attribute::Type>(_type);
}


void Attribute::setLength(uint16_t size)
{
    _size = size;
}


std::string Attribute::typeString()
{
    return typeString(_type);
}


void Attribute::consumePadding(BitReader& reader) const
{
    auto padding = paddingBytes();
    if (padding > 0)
        reader.skip(padding);
}


void Attribute::writePadding(BitWriter& writer) const
{
    auto padding = paddingBytes();
    if (padding > 0) {
        char zeroes[4] = {0};
        writer.put(zeroes, padding);
    }
}


std::unique_ptr<Attribute> Attribute::create(uint16_t type, uint16_t size)
{
    switch (type) {
        case Attribute::MappedAddress:
        case Attribute::ResponseAddress:
        case Attribute::SourceAddress:
        case Attribute::ChangedAddress:
        case Attribute::ReflectedFrom:
        case Attribute::AlternateServer:
        case Attribute::DestinationAddress:
        case Attribute::XorMappedAddress:
        case Attribute::XorPeerAddress:
        case Attribute::XorRelayedAddress: {
            if (!isAddressAttributeSize(size))
                return nullptr;

            std::unique_ptr<Attribute> attr;
            switch (type) {
                case Attribute::MappedAddress:
                    attr = std::make_unique<stun::MappedAddress>();
                    break;
                case Attribute::ResponseAddress:
                    attr = std::make_unique<stun::ResponseAddress>();
                    break;
                case Attribute::SourceAddress:
                    attr = std::make_unique<stun::SourceAddress>();
                    break;
                case Attribute::ChangedAddress:
                    attr = std::make_unique<stun::ChangedAddress>();
                    break;
                case Attribute::ReflectedFrom:
                    attr = std::make_unique<stun::ReflectedFrom>();
                    break;
                case Attribute::AlternateServer:
                    attr = std::make_unique<stun::AlternateServer>();
                    break;
                case Attribute::DestinationAddress:
                    attr = std::make_unique<stun::DestinationAddress>();
                    break;
                case Attribute::XorMappedAddress:
                    attr = std::make_unique<stun::XorMappedAddress>();
                    break;
                case Attribute::XorPeerAddress:
                    attr = std::make_unique<stun::XorPeerAddress>();
                    break;
                case Attribute::XorRelayedAddress:
                    attr = std::make_unique<stun::XorRelayedAddress>();
                    break;
            }
            attr->setLength(size);
            return attr;
        }

        case Attribute::ErrorCode:
            if (size < ErrorCode::MinSize)
                return nullptr;
            return std::make_unique<stun::ErrorCode>(size);

        case Attribute::UnknownAttributes:
            return size % sizeof(uint16_t) == 0 ?
                std::make_unique<stun::UnknownAttributes>(size) :
                nullptr;

        case Attribute::Fingerprint:
            return createFixedAttribute<stun::Fingerprint>(size);

        case Attribute::RequestedTransport:
            return createFixedAttribute<stun::RequestedTransport>(size);

        case Attribute::ChangeRequest:
            return createFixedAttribute<stun::ChangeRequest>(size);

        case Attribute::Lifetime:
            return createFixedAttribute<stun::Lifetime>(size);

        case Attribute::Bandwidth:
            return createFixedAttribute<stun::Bandwidth>(size);

        case Attribute::Options:
            return createFixedAttribute<stun::Options>(size);

        case Attribute::ChannelNumber:
            return createFixedAttribute<stun::ChannelNumber>(size);

        case Attribute::ConnectionID:
            return createFixedAttribute<stun::ConnectionID>(size);

        case Attribute::MessageIntegrity:
            return createFixedAttribute<stun::MessageIntegrity>(size);

        case Attribute::Nonce:
            return createBoundedAttribute<stun::Nonce>(size, 128);

        case Attribute::Realm:
            return createBoundedAttribute<stun::Realm>(size, 128);

        case Attribute::Software:
            return createBoundedAttribute<stun::Software>(size, 128);

        case Attribute::ReservationToken:
            return createExactSizeAttribute<stun::ReservationToken>(size, 8);

        case Attribute::MagicCookie:
            return createExactSizeAttribute<stun::MagicCookie>(size, 4);

        case Attribute::Data:
            return createVariableAttribute<stun::Data>(size);

        case Attribute::Username:
            return createBoundedAttribute<stun::Username>(size, 128);

        case Attribute::Password:
            return createBoundedAttribute<stun::Password>(size, 128);

        case Attribute::ICEPriority:
            return createFixedAttribute<stun::ICEPriority>(size);

        case Attribute::ICEControlled:
            return createFixedAttribute<stun::ICEControlled>(size);

        case Attribute::ICEControlling:
            return createFixedAttribute<stun::ICEControlling>(size);

        case Attribute::ICEUseCandidate:
            return (size == 0) ? std::make_unique<stun::ICEUseCandidate>() : nullptr;

        case Attribute::DontFragment:
            return createFixedAttribute<stun::DontFragment>(size);

        case Attribute::EventPort:
            return createFixedAttribute<stun::EventPort>(size);

        default:
            LTrace("Cannot create attribute for type: ", type, " size: ", size);
            break;
    }

    return nullptr;
}


// ---------------------------------------------------------------------------
//
AddressAttribute::AddressAttribute(uint16_t type, bool ipv4)
    : Attribute(type, ipv4 ? IPv4Size : IPv6Size)
{
}


AddressAttribute::AddressAttribute(const AddressAttribute& r)
    : Attribute(r._type, r._size)
    , _address(r._address)
{
}


std::unique_ptr<Attribute> AddressAttribute::clone()
{
    return std::make_unique<AddressAttribute>(*this);
}


net::Address AddressAttribute::address() const
{
    return _address;
}



void AddressAttribute::read(BitReader& reader)
{
    // X-Port is computed by taking the mapped port in host byte order,
    // XOR'ing it with the most significant 16 bits of the magic cookie, and
    // then the converting the result to network byte order.  If the IP
    // address family is IPv4, X-Address is computed by taking the mapped IP
    // address in host byte order, XOR'ing it with the magic cookie, and
    // converting the result to network byte order.  If the IP address
    // family is IPv6, X-Address is computed by taking the mapped IP address
    // in host byte order, XOR'ing it with the magic cookie and the 96-bit
    // transaction ID, and converting the result to network byte order.

    uint8_t dummy, family;
    reader.getU8(dummy);
    reader.getU8(family);

    // BitReader::getU16/getU32 convert from network to host byte order.
    // kMagicCookie is a host-order constant, so XOR directly in host order.
    bool xor_ = isXorType();

    uint16_t port;
    reader.getU16(port);
    if (xor_)
        port ^= static_cast<uint16_t>(kMagicCookie >> 16);

    if (family == static_cast<uint8_t>(AddressFamily::IPv4)) {
        if (size() != IPv4Size) {
            throw std::runtime_error("invalid IPv4 address attribute size");
        }

        uint32_t ip;
        reader.getU32(ip);
        if (xor_)
            ip ^= kMagicCookie;

        struct sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        sa.sin_addr.s_addr = htonl(ip);
        _address = net::Address(reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa));
    } else if (family == static_cast<uint8_t>(AddressFamily::IPv6)) {
        if (size() != IPv6Size) {
            throw std::runtime_error("invalid IPv6 address attribute size");
        }

        uint8_t raw[16];
        for (int i = 0; i < 16; i++) {
            uint8_t b;
            reader.getU8(b);
            raw[i] = b;
        }

        if (xor_) {
            const uint8_t* msgBegin = reinterpret_cast<const uint8_t*>(reader.begin());
            uint8_t xorMask[16];
            xorMask[0] = static_cast<uint8_t>((kMagicCookie >> 24) & 0xff);
            xorMask[1] = static_cast<uint8_t>((kMagicCookie >> 16) & 0xff);
            xorMask[2] = static_cast<uint8_t>((kMagicCookie >> 8) & 0xff);
            xorMask[3] = static_cast<uint8_t>(kMagicCookie & 0xff);
            std::memcpy(xorMask + 4, msgBegin + kTransactionIdOffset, kTransactionIdLength);

            for (int i = 0; i < 16; i++)
                raw[i] ^= xorMask[i];
        }

        struct sockaddr_in6 sa6{};
        sa6.sin6_family = AF_INET6;
        sa6.sin6_port = htons(port);
        std::memcpy(&sa6.sin6_addr, raw, 16);
        _address = net::Address(reinterpret_cast<struct sockaddr*>(&sa6), sizeof(sa6));
    } else {
        throw std::runtime_error("invalid address family in STUN attribute");
    }
}


void AddressAttribute::write(BitWriter& writer) const
{
    writer.putU8(0);
    writer.putU8(static_cast<uint8_t>(family()));

    bool xor_ = isXorType();

    switch (_address.family()) {
        case net::Address::IPv4: {
            auto v4addr = reinterpret_cast<sockaddr_in*>(
                const_cast<sockaddr*>(_address.addr()));

            uint16_t port = ntohs(v4addr->sin_port);
            if (xor_)
                port ^= static_cast<uint16_t>(kMagicCookie >> 16);
            writer.putU16(port);

            uint32_t ip = ntohl(v4addr->sin_addr.s_addr);
            if (xor_)
                ip ^= kMagicCookie;
            writer.putU32(ip);
            break;
        }
        case net::Address::IPv6: {
            auto v6addr = reinterpret_cast<sockaddr_in6*>(
                const_cast<sockaddr*>(_address.addr()));

            uint16_t port = ntohs(v6addr->sin6_port);
            if (xor_)
                port ^= static_cast<uint16_t>(kMagicCookie >> 16);
            writer.putU16(port);

            const uint8_t* addrBytes = reinterpret_cast<const uint8_t*>(
                &v6addr->sin6_addr);

            if (xor_) {
                const uint8_t* msgBegin = reinterpret_cast<const uint8_t*>(writer.begin());
                uint8_t xorMask[16];
                xorMask[0] = static_cast<uint8_t>((kMagicCookie >> 24) & 0xff);
                xorMask[1] = static_cast<uint8_t>((kMagicCookie >> 16) & 0xff);
                xorMask[2] = static_cast<uint8_t>((kMagicCookie >> 8) & 0xff);
                xorMask[3] = static_cast<uint8_t>(kMagicCookie & 0xff);
                std::memcpy(xorMask + 4, msgBegin + kTransactionIdOffset, kTransactionIdLength);

                for (int i = 0; i < 16; i++)
                    writer.putU8(addrBytes[i] ^ xorMask[i]);
            } else {
                for (int i = 0; i < 16; i++)
                    writer.putU8(addrBytes[i]);
            }
            break;
        }
    }
}


// ---------------------------------------------------------------------------
//
UInt8Attribute::UInt8Attribute(uint16_t type)
    : Attribute(type, Size)
    , _bits(0)
{
}


UInt8Attribute::UInt8Attribute(const UInt8Attribute& r)
    : Attribute(r._type, Size)
    , _bits(r._bits)
{
}


std::unique_ptr<Attribute> UInt8Attribute::clone()
{
    return std::make_unique<UInt8Attribute>(*this);
}


bool UInt8Attribute::getBit(int index) const
{
    if (index < 0 || index >= 8)
        throw std::runtime_error("UInt8Attribute::getBit: index out of range");
    return static_cast<bool>((_bits >> index) & 0x1);
}


void UInt8Attribute::setBit(int index, bool value)
{
    if (index < 0 || index >= 8)
        throw std::runtime_error("UInt8Attribute::setBit: index out of range");
    _bits &= ~(1 << index);
    _bits |= value ? (1 << index) : 0;
}


void UInt8Attribute::read(BitReader& reader)
{
    reader.getU8(_bits);
    consumePadding(reader);
}


void UInt8Attribute::write(BitWriter& writer) const
{
    writer.putU8(_bits);
    writePadding(writer);
}


// ---------------------------------------------------------------------------
//
UInt32Attribute::UInt32Attribute(uint16_t type)
    : Attribute(type, Size)
    , _bits(0)
{
}


UInt32Attribute::UInt32Attribute(const UInt32Attribute& r)
    : Attribute(r._type, Size)
    , _bits(r._bits)
{
}


std::unique_ptr<Attribute> UInt32Attribute::clone()
{
    return std::make_unique<UInt32Attribute>(*this);
}


bool UInt32Attribute::getBit(int index) const
{
    if (index < 0 || index >= 32)
        throw std::runtime_error("UInt32Attribute::getBit: index out of range");
    return static_cast<bool>((_bits >> index) & 0x1);
}


void UInt32Attribute::setBit(int index, bool value)
{
    if (index < 0 || index >= 32)
        throw std::runtime_error("UInt32Attribute::setBit: index out of range");
    _bits &= ~(1u << index);
    _bits |= value ? (1u << index) : 0;
}


void UInt32Attribute::read(BitReader& reader)
{
    reader.getU32(_bits);
}

void UInt32Attribute::write(BitWriter& writer) const
{
    writer.putU32(_bits);
}


// ---------------------------------------------------------------------------
//
UInt64Attribute::UInt64Attribute(uint16_t type)
    : Attribute(type, Size)
    , _bits(0)
{
}


UInt64Attribute::UInt64Attribute(const UInt64Attribute& r)
    : Attribute(r._type, Size)
    , _bits(r._bits)
{
}


std::unique_ptr<Attribute> UInt64Attribute::clone()
{
    return std::make_unique<UInt64Attribute>(*this);
}


bool UInt64Attribute::getBit(int index) const
{
    if (index < 0 || index >= 64)
        throw std::runtime_error("UInt64Attribute::getBit: index out of range");
    return static_cast<bool>((_bits >> index) & 0x1);
}


void UInt64Attribute::setBit(int index, bool value)
{
    if (index < 0 || index >= 64)
        throw std::runtime_error("UInt64Attribute::setBit: index out of range");
    _bits &= ~(1ull << index);
    _bits |= value ? (1ull << index) : 0;
}


void UInt64Attribute::read(BitReader& reader)
{
    reader.getU64(_bits);
}


void UInt64Attribute::write(BitWriter& writer) const
{
    writer.putU64(_bits);
}


// ---------------------------------------------------------------------------
//
FlagAttribute::FlagAttribute(uint16_t type)
    : Attribute(type, 0)
{
}


std::unique_ptr<Attribute> FlagAttribute::clone()
{
    return std::make_unique<FlagAttribute>(type());
}


// ---------------------------------------------------------------------------
//
StringAttribute::StringAttribute(uint16_t type, uint16_t size)
    : Attribute(type, size)
{
}


StringAttribute::StringAttribute(const StringAttribute& r)
    : Attribute(r._type, r._size)
    , _bytes(r._bytes)
{
}


StringAttribute::~StringAttribute()
{
}


std::unique_ptr<Attribute> StringAttribute::clone()
{
    return std::make_unique<StringAttribute>(*this);
}


void StringAttribute::setBytes(const char* bytes, unsigned size)
{
    _bytes.assign(bytes, bytes + size);
    setLength(size);
}


void StringAttribute::copyBytes(const char* bytes)
{
    copyBytes(bytes, static_cast<uint16_t>(std::strlen(bytes)));
}


void StringAttribute::copyBytes(const void* bytes, unsigned size)
{
    const char* src = static_cast<const char*>(bytes);
    _bytes.assign(src, src + size);
    setLength(size);
}


uint8_t StringAttribute::getByte(int index) const
{
    if (_bytes.empty())
        throw std::runtime_error("StringAttribute::getByte: no data");
    if (index < 0 || index >= static_cast<int>(Attribute::size()))
        throw std::runtime_error("StringAttribute::getByte: index out of range");
    return static_cast<uint8_t>(_bytes[index]);
}


void StringAttribute::setByte(int index, uint8_t value)
{
    if (_bytes.empty())
        throw std::runtime_error("StringAttribute::setByte: no data");
    if (index < 0 || index >= static_cast<int>(Attribute::size()))
        throw std::runtime_error("StringAttribute::setByte: index out of range");
    _bytes[index] = static_cast<char>(value);
}


void StringAttribute::read(BitReader& reader)
{
    _bytes.resize(Attribute::size());
    reader.get(_bytes.data(), Attribute::size());

    consumePadding(reader);
}


void StringAttribute::write(BitWriter& writer) const
{
    if (!_bytes.empty())
        writer.put(_bytes.data(), Attribute::size());

    writePadding(writer);
}


std::string StringAttribute::asString() const
{
    return std::string(_bytes.data(), Attribute::size());
}


// ---------------------------------------------------------------------------
//
MessageIntegrity::MessageIntegrity()
    : Attribute(Attribute::MessageIntegrity, Size)
{
}


MessageIntegrity::MessageIntegrity(const MessageIntegrity& r)
    : Attribute(r._type, Size)
    , _input(r._input)
    , _hmac(r._hmac)
    , _key(r._key)
{
}


MessageIntegrity::~MessageIntegrity()
{
}


std::unique_ptr<Attribute> MessageIntegrity::clone()
{
    return std::make_unique<MessageIntegrity>(*this);
}


bool MessageIntegrity::verifyHmac(std::string_view key) const
{
    if (key.empty())
        throw std::runtime_error("MessageIntegrity::verifyHmac: empty key");
    if (_hmac.empty())
        throw std::runtime_error("MessageIntegrity::verifyHmac: empty hmac");
    if (_input.empty())
        throw std::runtime_error("MessageIntegrity::verifyHmac: empty input");

    std::string hmac = crypto::computeHMAC(_input, key);
    if (hmac.size() != MessageIntegrity::Size)
        throw std::runtime_error("MessageIntegrity::verifyHmac: unexpected HMAC size");

    return _hmac == hmac;
}


void MessageIntegrity::read(BitReader& reader)
{
    const size_t hmacValueOffset = reader.position();
    const size_t hmacHeaderOffset = hmacValueOffset - kAttributeHeaderSize;

    // RFC 5389 section 15.4: the HMAC input stops before the
    // MESSAGE-INTEGRITY attribute, but the STUN header length must be
    // adjusted as if the attribute were present.
    Buffer hmacBuf;
    hmacBuf.reserve(std::max(hmacHeaderOffset, size_t(256)));
    BitWriter hmacWriter(hmacBuf.data(), hmacBuf.capacity());
    hmacWriter.put(reader.begin(), hmacHeaderOffset);

    const size_t messageLength =
        hmacHeaderOffset + kAttributeHeaderSize + MessageIntegrity::Size - kMessageHeaderSize;
    hmacWriter.updateU16(static_cast<uint16_t>(messageLength), 2);
    _input.assign(hmacWriter.begin(), hmacWriter.position());

    _hmac.assign(reader.current(), MessageIntegrity::Size);

    reader.skip(MessageIntegrity::Size);
}


void MessageIntegrity::write(BitWriter& writer) const
{
    // If the key (password) is present then compute the HMAC
    // for the current message, otherwise the attribute content
    // will be copied.
    if (!_key.empty()) {
        const size_t hmacValueOffset = writer.position();
        const size_t hmacHeaderOffset = hmacValueOffset - kAttributeHeaderSize;

        Buffer hmacBuf;
        hmacBuf.reserve(std::max(hmacHeaderOffset, size_t(256)));
        BitWriter hmacWriter(hmacBuf.data(), hmacBuf.capacity());
        hmacWriter.put(writer.begin(), hmacHeaderOffset);

        // RFC 5389 section 15.4: hash the message up to the attribute
        // preceding MESSAGE-INTEGRITY, but set the STUN header length as if
        // MESSAGE-INTEGRITY were present and no following attributes existed.
        const size_t messageLength =
            hmacHeaderOffset + kAttributeHeaderSize + MessageIntegrity::Size - kMessageHeaderSize;
        hmacWriter.updateU16(static_cast<uint16_t>(messageLength), 2);

        std::string input(hmacWriter.begin(), hmacWriter.position());
        if (input.size() != hmacHeaderOffset)
            throw std::runtime_error("MessageIntegrity::write: input size mismatch");

        std::string hmac(crypto::computeHMAC(input, _key));
        if (hmac.size() != MessageIntegrity::Size)
            throw std::runtime_error("MessageIntegrity::write: unexpected HMAC size");

        // Append the real HMAC to the buffer.
        writer.put(hmac.c_str(), hmac.length());

    } else {
        if (_hmac.size() != MessageIntegrity::Size)
            throw std::runtime_error("MessageIntegrity::write: HMAC not set");
        writer.put(_hmac.c_str(), MessageIntegrity::Size);
    }
}


// ---------------------------------------------------------------------------
//
ErrorCode::ErrorCode(uint16_t size)
    : Attribute(Attribute::ErrorCode, size)
    , _class(0)
    , _number(0)
{
    if (size < MinSize)
        throw std::runtime_error("ErrorCode: size too small");
}


ErrorCode::ErrorCode(const ErrorCode& r)
    : Attribute(Attribute::ErrorCode, r._size)
    , _class(r._class)
    , _number(r._number)
    , _reason(r._reason)
{
}


ErrorCode::~ErrorCode()
{
}


std::unique_ptr<Attribute> ErrorCode::clone()
{
    return std::make_unique<ErrorCode>(*this);
}


int ErrorCode::errorCode() const
{
    return _class * 100 + _number;
}


void ErrorCode::setErrorCode(int code)
{
    _class = static_cast<uint8_t>(code / 100);
    _number = static_cast<uint8_t>(code % 100);
}


void ErrorCode::setReason(const std::string& reason)
{
    setLength(MinSize + static_cast<uint16_t>(reason.size()));
    _reason = reason;
}


void ErrorCode::read(BitReader& reader)
{
    uint32_t val;
    reader.getU32(val);

    if ((val >> 11) != 0)
        throw std::runtime_error("error-code bits not zero");

    _class = ((val >> 8) & 0x7);
    _number = (val & 0xff);

    reader.get(_reason, size() - 4);
    consumePadding(reader);
}


void ErrorCode::write(BitWriter& writer) const
{
    writer.putU32(_class << 8 | _number); // errorCode());
    writer.put(_reason);
    writePadding(writer);
}


// ---------------------------------------------------------------------------
//
UInt16ListAttribute::UInt16ListAttribute(uint16_t type, uint16_t size)
    : Attribute(type, size)
{
}


UInt16ListAttribute::UInt16ListAttribute(const UInt16ListAttribute& r)
    : Attribute(r._type, r._size)
    , _attrTypes(r._attrTypes)
{
}


UInt16ListAttribute::~UInt16ListAttribute()
{
}


std::unique_ptr<Attribute> UInt16ListAttribute::clone()
{
    return std::make_unique<UInt16ListAttribute>(*this);
}


size_t UInt16ListAttribute::size() const
{
    return _attrTypes.size();
}


uint16_t UInt16ListAttribute::getType(int index) const
{
    return _attrTypes[index];
}


void UInt16ListAttribute::setType(int index, uint16_t value)
{
    _attrTypes[index] = value;
}


void UInt16ListAttribute::addType(uint16_t value)
{
    _attrTypes.push_back(value);
    setLength(static_cast<uint16_t>(_attrTypes.size() * 2));
}


void UInt16ListAttribute::read(BitReader& reader)
{
    for (size_t i = 0; i < Attribute::size() / 2; i++) {
        uint16_t attr;
        reader.getU16(attr);
        _attrTypes.push_back(attr);
    }

    // Padding of these attributes is done in RFC 5389 style. This is
    // slightly different from RFC3489, but it shouldn't be important.
    // RFC3489 pads out to a 32 bit boundary by duplicating one of the
    // entries in the list (not necessarily the last one - it's unspecified).
    // RFC5389 pads on the end, and the bytes are always ignored.
    consumePadding(reader);
}

void UInt16ListAttribute::write(BitWriter& writer) const
{
    for (const auto& attrType : _attrTypes)
        writer.putU16(attrType);
    writePadding(writer);
}


} // namespace stun
} // namespace icy


/// @}
