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

#include <cstdio>
#include <cstring>
#include <stdexcept>


namespace icy {
namespace stun {


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
            return "REFLECTED-FORM";
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
            return "RESERVED-TOKEN";
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
    int remainder = _size % 4;
    if (remainder > 0) {
        reader.skip(4 - remainder);
    }
}


void Attribute::writePadding(BitWriter& writer) const
{
    int remainder = _size % 4;
    if (remainder > 0) {
        char zeroes[4] = {0};
        writer.put(zeroes, 4 - remainder);
    }
}


std::unique_ptr<Attribute> Attribute::create(uint16_t type, uint16_t size)
{
    switch (type) {
        case Attribute::MappedAddress:
            if (size != AddressAttribute::IPv4Size &&
                size != AddressAttribute::IPv6Size)
                return nullptr;
            return std::make_unique<stun::MappedAddress>();

        case Attribute::XorMappedAddress:
            if (size != AddressAttribute::IPv4Size &&
                size != AddressAttribute::IPv6Size)
                return nullptr;
            return std::make_unique<stun::XorMappedAddress>();

        case Attribute::XorRelayedAddress:
            if (size != AddressAttribute::IPv4Size &&
                size != AddressAttribute::IPv6Size)
                return nullptr;
            return std::make_unique<stun::XorRelayedAddress>();

        case Attribute::XorPeerAddress:
            if (size != AddressAttribute::IPv4Size &&
                size != AddressAttribute::IPv6Size)
                return nullptr;
            return std::make_unique<stun::XorPeerAddress>();

        case Attribute::AlternateServer:
            if (size != AddressAttribute::IPv4Size &&
                size != AddressAttribute::IPv6Size)
                return nullptr;
            return std::make_unique<stun::AlternateServer>();

        case Attribute::ErrorCode:
            if (size < ErrorCode::MinSize)
                return nullptr;
            return std::make_unique<stun::ErrorCode>(size);

        case Attribute::UnknownAttributes:
            return std::make_unique<stun::UnknownAttributes>(size);

        case Attribute::Fingerprint:
            if (size != Fingerprint::Size)
                return nullptr;
            return std::make_unique<stun::Fingerprint>();

        case Attribute::RequestedTransport:
            if (size != RequestedTransport::Size)
                return nullptr;
            return std::make_unique<stun::RequestedTransport>();

        case Attribute::Lifetime:
            if (size != Lifetime::Size)
                return nullptr;
            return std::make_unique<stun::Lifetime>();

        case Attribute::Bandwidth:
            if (size != Bandwidth::Size)
                return nullptr;
            return std::make_unique<stun::Bandwidth>();

        case Attribute::ChannelNumber:
            if (size != ChannelNumber::Size)
                return nullptr;
            return std::make_unique<stun::ChannelNumber>();

        case Attribute::ConnectionID:
            if (size != ConnectionID::Size)
                return nullptr;
            return std::make_unique<stun::ConnectionID>();

        case Attribute::MessageIntegrity:
            return (size == 20) ? std::make_unique<stun::MessageIntegrity>() : nullptr;

        case Attribute::Nonce:
            return (size <= 128) ? std::make_unique<stun::Nonce>(size) : nullptr;

        case Attribute::Realm:
            return (size <= 128) ? std::make_unique<stun::Realm>(size) : nullptr;

        case Attribute::Software:
            return (size <= 128) ? std::make_unique<stun::Software>(size) : nullptr;

        case Attribute::ReservationToken:
            return (size == 8) ? std::make_unique<stun::ReservationToken>() : nullptr;

        case Attribute::MagicCookie:
            return (size == 4) ? std::make_unique<stun::MagicCookie>() : nullptr;

        case Attribute::Data:
            return std::make_unique<stun::Data>(size);

        case Attribute::Username:
            return (size <= 128) ? std::make_unique<stun::Username>(size) : nullptr;

        case Attribute::Password:
            return (size <= 128) ? std::make_unique<stun::Password>(size) : nullptr;

        case Attribute::ICEPriority:
            if (size != ICEPriority::Size)
                return nullptr;
            return std::make_unique<stun::ICEPriority>();

        case Attribute::ICEControlled:
            if (size != ICEControlled::Size)
                return nullptr;
            return std::make_unique<stun::ICEControlled>();

        case Attribute::ICEControlling:
            if (size != ICEControlling::Size)
                return nullptr;
            return std::make_unique<stun::ICEControlling>();

        case Attribute::ICEUseCandidate:
            return (size == 0) ? std::make_unique<stun::ICEUseCandidate>() : nullptr;

        case Attribute::DontFragment:
            if (size != DontFragment::Size)
                return nullptr;
            return std::make_unique<stun::DontFragment>();

        case Attribute::EventPort:
            if (size != EventPort::Size)
                return nullptr;
            return std::make_unique<stun::EventPort>();

        default:
            LError("Cannot create attribute for type: ", type);
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


std::string intToIPv4(uint32_t ip)
{
    // Input in host byte order; high byte = first octet
    char str[20];
    std::snprintf(str, sizeof(str), "%d.%d.%d.%d",
                  (ip >> 24) & 0xff, (ip >> 16) & 0xff,
                  (ip >> 8) & 0xff, ip & 0xff);
    return std::string(str);
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
    uint16_t port;
    reader.getU16(port);
    port ^= static_cast<uint16_t>(kMagicCookie >> 16);

    if (family == static_cast<uint8_t>(AddressFamily::IPv4)) {
        if (size() != IPv4Size) {
            throw std::runtime_error("invalid IPv4 address attribute size");
        }

        uint32_t ip;
        reader.getU32(ip);
        ip ^= kMagicCookie;

        _address = net::Address(intToIPv4(ip), port);
    } else if (family == static_cast<uint8_t>(AddressFamily::IPv6)) {
        if (size() != IPv6Size) {
            throw std::runtime_error("invalid IPv6 address attribute size");
        }

        // RFC 5389 Section 15.2: For IPv6, X-Address is computed by taking
        // the mapped IP address in host byte order, XOR'ing it with the
        // concatenation of the magic cookie (32 bits) and the transaction
        // ID (96 bits), and converting the result to network byte order.

        // Read the 16 raw bytes of the IPv6 address from the wire
        uint8_t xored[16];
        for (int i = 0; i < 16; i++) {
            uint8_t b;
            reader.getU8(b);
            xored[i] = b;
        }

        // Build the 16-byte XOR mask: magic cookie (4 bytes) + transaction ID (12 bytes)
        // We need to get the transaction ID from the message header which
        // starts at offset 8 in the reader's buffer (after type + length + cookie).
        // The reader.begin() points to the start of the STUN message.
        const uint8_t* msgBegin = reinterpret_cast<const uint8_t*>(reader.begin());
        uint8_t xorMask[16];
        // First 4 bytes: magic cookie in network byte order
        xorMask[0] = static_cast<uint8_t>((kMagicCookie >> 24) & 0xff);
        xorMask[1] = static_cast<uint8_t>((kMagicCookie >> 16) & 0xff);
        xorMask[2] = static_cast<uint8_t>((kMagicCookie >> 8) & 0xff);
        xorMask[3] = static_cast<uint8_t>(kMagicCookie & 0xff);
        // Next 12 bytes: transaction ID from offset 8 in the message
        std::memcpy(xorMask + 4, msgBegin + kTransactionIdOffset, kTransactionIdLength);

        // XOR the address bytes with the mask
        uint8_t addr[16];
        for (int i = 0; i < 16; i++) {
            addr[i] = xored[i] ^ xorMask[i];
        }

        // Build IPv6 address string from the decoded bytes
        char addrStr[64];
        std::snprintf(addrStr, sizeof(addrStr),
                      "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                      "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                      addr[0], addr[1], addr[2], addr[3],
                      addr[4], addr[5], addr[6], addr[7],
                      addr[8], addr[9], addr[10], addr[11],
                      addr[12], addr[13], addr[14], addr[15]);

        _address = net::Address(std::string(addrStr), port);
    } else {
        throw std::runtime_error("invalid address family in STUN attribute");
    }
}


void AddressAttribute::write(BitWriter& writer) const
{
    writer.putU8(0);
    writer.putU8(static_cast<uint8_t>(family()));

    // BitWriter::putU16/putU32 convert from host to network byte order.
    // kMagicCookie is a host-order constant; sockaddr fields are in network order.
    switch (_address.family()) {
        case net::Address::IPv4: {
            auto v4addr = reinterpret_cast<sockaddr_in*>(
                const_cast<sockaddr*>(_address.addr()));

            // Port: convert from network to host, XOR, then putU16 converts back
            uint16_t port = ntohs(v4addr->sin_port);
            port ^= static_cast<uint16_t>(kMagicCookie >> 16);
            writer.putU16(port);

            // Address: convert from network to host, XOR, then putU32 converts back
            uint32_t ip = ntohl(v4addr->sin_addr.s_addr);
            ip ^= kMagicCookie;
            writer.putU32(ip);
            break;
        }
        case net::Address::IPv6: {
            auto v6addr = reinterpret_cast<sockaddr_in6*>(
                const_cast<sockaddr*>(_address.addr()));

            // Port: convert from network to host, XOR, then putU16 converts back
            uint16_t port = ntohs(v6addr->sin6_port);
            port ^= static_cast<uint16_t>(kMagicCookie >> 16);
            writer.putU16(port);

            // Address: XOR with magic cookie + transaction ID
            // Build the XOR mask from the message header already written
            const uint8_t* msgBegin = reinterpret_cast<const uint8_t*>(writer.begin());
            uint8_t xorMask[16];
            xorMask[0] = static_cast<uint8_t>((kMagicCookie >> 24) & 0xff);
            xorMask[1] = static_cast<uint8_t>((kMagicCookie >> 16) & 0xff);
            xorMask[2] = static_cast<uint8_t>((kMagicCookie >> 8) & 0xff);
            xorMask[3] = static_cast<uint8_t>(kMagicCookie & 0xff);
            std::memcpy(xorMask + 4, msgBegin + kTransactionIdOffset, kTransactionIdLength);

            // Get raw IPv6 address bytes in network order
            const uint8_t* addrBytes = reinterpret_cast<const uint8_t*>(
                &v6addr->sin6_addr);

            // XOR each byte and write
            for (int i = 0; i < 16; i++) {
                writer.putU8(addrBytes[i] ^ xorMask[i]);
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
    int sizeBeforeMessageIntegrity = reader.position() - kAttributeHeaderSize;

    // Get the message prior to the current attribute and fill the
    // attribute with dummy content.
    Buffer hmacBuf(sizeBeforeMessageIntegrity);
    BitWriter hmacWriter(hmacBuf);

    hmacWriter.put(reader.begin(), reader.position() - kAttributeHeaderSize);

    // Ensure the STUN message size reflects the message up to and
    // including the MessageIntegrity attribute.
    hmacWriter.updateU32(static_cast<uint32_t>(sizeBeforeMessageIntegrity) + MessageIntegrity::Size, 2);
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

        // The hash used to construct MESSAGE-INTEGRITY includes the length
        // field from the STUN message header.
        // Prior to performing the hash, the MESSAGE-INTEGRITY attribute MUST be
        // inserted into the message (with dummy content).
        int sizeBeforeMessageIntegrity = writer.position() - kAttributeHeaderSize;

        // Get the message prior to the current attribute and
        // fill the attribute with dummy content.
        Buffer hmacBuf(sizeBeforeMessageIntegrity);
        BitWriter hmacWriter(hmacBuf);
        hmacWriter.put(writer.begin(), sizeBeforeMessageIntegrity);

        // The length MUST then
        // be set to point to the length of the message up to, and including,
        // the MESSAGE-INTEGRITY attribute itself, but excluding any attributes
        // after it.  Once the computation is performed, the value of the
        // MESSAGE-INTEGRITY attribute can be filled in, and the value of the
        // length in the STUN header can be set to its correct value -- the
        // length of the entire message.  Similarly, when validating the
        // MESSAGE-INTEGRITY, the length field should be adjusted to point to
        // the end of the MESSAGE-INTEGRITY attribute prior to calculating the
        // HMAC.  Such adjustment is necessary when attributes, such as
        // FINGERPRINT, appear after MESSAGE-INTEGRITY.
        hmacWriter.updateU32(static_cast<uint32_t>(sizeBeforeMessageIntegrity) + MessageIntegrity::Size, 2);

        std::string input(hmacWriter.begin(), hmacWriter.position());
        if (input.size() != static_cast<size_t>(sizeBeforeMessageIntegrity))
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
    for (size_t i = 0; i < size() / 2; i++) {
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


/// @\}
