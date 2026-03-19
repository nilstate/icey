///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup stun
/// @{


#pragma once


#include "icy/buffer.h"
#include "icy/crypto/crypto.h"
#include "icy/net/address.h"
#include "icy/stun/stun.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>


namespace icy {
namespace stun {


/// The virtual base class for all STUN/TURN attributes.
class STUN_API Attribute
{
public:
    enum Type
    {
        NotExist = 0,
        MappedAddress = 0x0001,
        ResponseAddress = 0x0002, // Not implemented
        ChangeRequest = 0x0003,   // Not implemented
        SourceAddress = 0x0004,   // Not implemented
        ChangedAddress = 0x0005,  // Not implemented
        Username = 0x0006,
        Password = 0x0007, // Not implemented
        MessageIntegrity = 0x0008,
        ErrorCode = 0x0009,
        Bandwidth = 0x0010,          // Not implemented
        DestinationAddress = 0x0011, // Not implemented
        UnknownAttributes = 0x000a,
        ReflectedFrom = 0x000b, // Not implemented
        // TransportPreferences    = 0x000c, // Not implemented
        MagicCookie = 0x000f, // Not implemented, ByteString, 4 bytes
        Realm = 0x0014,
        Nonce = 0x0015,
        XorMappedAddress = 0x0020,
        Software = 0x8022,
        Options = 0x8001, // Not implemented
        AlternateServer = 0x000e,
        Fingerprint = 0x8028, /// TURN
        ChannelNumber = 0x000c,
        Lifetime = 0x000d, // 0x0010: Reserved (was BANDWIDTH)
        XorPeerAddress = 0x0012,
        Data = 0x0013,
        XorRelayedAddress = 0x0016,
        EventPort = 0x0018, // Not implemented
        RequestedTransport = 0x0019,
        DontFragment = 0x001A, // Not implemented
        /// 0x0021: Reserved (was TIMER-VAL)
        ReservationToken = 0x0022, // 8 bytes token value

        /// TURN TCP
        ConnectionID = 0x002a,

        /// ICE
        ICEControlled = 0x8029,
        ICEControlling = 0x802A,
        ICEPriority = 0x0024,
        ICEUseCandidate = 0x0025
    };

    virtual ~Attribute() {}
    virtual std::unique_ptr<Attribute> clone() = 0;

    /// Reads the body (not the type or size) for this
    /// type of attribute from  the given buffer. Return
    /// value is true if successful.
    virtual void read(BitReader& reader) = 0;

    /// Writes the body (not the type or size) to the
    /// given buffer. Return value is true if successful.
    virtual void write(BitWriter& writer) const = 0;

    /// Creates an attribute object with the given type
    /// and size.
    [[nodiscard]] static std::unique_ptr<Attribute> create(uint16_t type, uint16_t size = 0);

    [[nodiscard]] uint16_t type() const;
    [[nodiscard]] uint16_t size() const;

    void consumePadding(BitReader& reader) const;
    void writePadding(BitWriter& writer) const;

    static constexpr uint16_t TypeID = 0;

    [[nodiscard]] std::string typeString();
    [[nodiscard]] static std::string typeString(uint16_t type);

protected:
    Attribute(uint16_t type, uint16_t size = 0);
    void setLength(uint16_t size);

    uint16_t _type;
    uint16_t _size;
};


///
/// Implements a STUN/TURN attribute that contains a socket address.
class STUN_API AddressAttribute : public Attribute
{
public:
    AddressAttribute(uint16_t type, bool ipv4 = true); // bool xor,
    AddressAttribute(const AddressAttribute& r);

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t IPv4Size = 8;
    static constexpr uint16_t IPv6Size = 20;

    [[nodiscard]] stun::AddressFamily family() const
    {
        switch (_address.family()) {
            case net::Address::IPv4:
                return stun::AddressFamily::IPv4;
            case net::Address::IPv6:
                return stun::AddressFamily::IPv6;
        }
        return stun::AddressFamily::Undefined;
    }

    [[nodiscard]] virtual net::Address address() const;

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

    virtual void setAddress(const net::Address& addr) { _address = addr; }

private:
    net::Address _address;
};


///
/// Implements STUN/TURN attribute that reflects a 32-bit integer.
class STUN_API UInt8Attribute : public Attribute
{
public:
    UInt8Attribute(uint16_t type);
    UInt8Attribute(const UInt8Attribute& r);

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t Size = 1;

    [[nodiscard]] uint8_t value() const { return _bits; }
    void setValue(uint8_t bits) { _bits = bits; }

    [[nodiscard]] bool getBit(int index) const;
    void setBit(int index, bool value);

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    uint8_t _bits;
};


///
/// Implements STUN/TURN attribute that reflects a 32-bit integer.
class STUN_API UInt32Attribute : public Attribute
{
public:
    UInt32Attribute(uint16_t type);
    UInt32Attribute(const UInt32Attribute& r);

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t Size = 4;

    [[nodiscard]] uint32_t value() const { return _bits; }
    void setValue(uint32_t bits) { _bits = bits; }

    [[nodiscard]] bool getBit(int index) const;
    void setBit(int index, bool value);

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    uint32_t _bits;
};


///
/// Implements STUN/TURN attribute that reflects a 64-bit integer.
class STUN_API UInt64Attribute : public Attribute
{
public:
    UInt64Attribute(uint16_t type);
    UInt64Attribute(const UInt64Attribute& r);

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t Size = 8;

    [[nodiscard]] uint64_t value() const { return _bits; }
    void setValue(uint64_t bits) { _bits = bits; }

    [[nodiscard]] bool getBit(int index) const;
    void setBit(int index, bool value);

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    uint64_t _bits;
};

/// Implements STUN/TURN attribute representing a 0 size flag.
class STUN_API FlagAttribute : public Attribute
{
public:
    FlagAttribute(uint16_t type);

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t Size = 0;

    void read(BitReader&) override { /* flags have no body */ }
    void write(BitWriter&) const override { /* flags have no body */ }
};


///
/// Implements STUN/TURN attribute that reflects an arbitrary byte string
class STUN_API StringAttribute : public Attribute
{
public:
    StringAttribute(uint16_t type, uint16_t size = 0);
    StringAttribute(const StringAttribute& r);
    virtual ~StringAttribute();

    std::unique_ptr<Attribute> clone() override;

    [[nodiscard]] const char* bytes() const { return _bytes.data(); }
    void setBytes(const char* bytes, unsigned size);

    [[nodiscard]] std::string asString() const;
    void copyBytes(const char* bytes); //  uses strlen
    void copyBytes(const void* bytes, unsigned size);

    [[nodiscard]] uint8_t getByte(int index) const;
    void setByte(int index, uint8_t value);

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    std::vector<char> _bytes;
};


///
/// Implements STUN/TURN attribute that reflects a list of attribute names.
class STUN_API UInt16ListAttribute : public Attribute
{
public:
    UInt16ListAttribute(uint16_t type, uint16_t size);
    UInt16ListAttribute(const UInt16ListAttribute& r);
    virtual ~UInt16ListAttribute();

    std::unique_ptr<Attribute> clone() override;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] uint16_t getType(int index) const;
    void setType(int index, uint16_t value);
    void addType(uint16_t value);

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    std::vector<uint16_t> _attrTypes;
};


///
/// Implements STUN/TURN attributes that reflects an internet address.
class STUN_API MessageIntegrity : public Attribute
{
public:
    MessageIntegrity();
    MessageIntegrity(const MessageIntegrity& r);
    virtual ~MessageIntegrity();

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t TypeID = 0x0008;
    static constexpr uint16_t Size = 20;

    [[nodiscard]] bool verifyHmac(std::string_view key) const;

    [[nodiscard]] std::string input() const { return _input; }
    [[nodiscard]] std::string hmac() const { return _hmac; }
    [[nodiscard]] std::string key() const { return _key; }

    void setInput(const std::string& input) { _input = input; }
    void setHmac(const std::string& hmac) { _hmac = hmac; }
    void setKey(const std::string& key) { _key = key; }

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    std::string _input;
    std::string _hmac;
    std::string _key;
};


///
/// Implements STUN/TURN attribute that reflects an error code.
class STUN_API ErrorCode : public Attribute
{
public:
    ErrorCode(uint16_t size = MinSize);
    ErrorCode(const ErrorCode& r);
    virtual ~ErrorCode();

    std::unique_ptr<Attribute> clone() override;

    static constexpr uint16_t TypeID = 0x0009;
    static constexpr uint16_t MinSize = 4;

    void setErrorCode(int code);
    // void setErrorClass(uint8_t eClass);
    // void setErrorNumber(uint8_t eNumber);
    void setReason(const std::string& reason);

    [[nodiscard]] int errorCode() const;
    [[nodiscard]] uint8_t errorClass() const { return _class; }
    [[nodiscard]] uint8_t errorNumber() const { return _number; }
    [[nodiscard]] const std::string& reason() const { return _reason; }

    void read(BitReader& reader) override;
    void write(BitWriter& writer) const override;

private:
    uint8_t _class;
    uint8_t _number;
    std::string _reason;
};


//
/// Attribute macros
//

#define DECLARE_FIXLEN_STUN_ATTRIBUTE(Name, Type, Derives) \
                                                           \
    class Name : public Derives                            \
    {                                                      \
    public:                                                \
        static constexpr uint16_t TypeID = Type;           \
        Name()                                             \
            : Derives(TypeID) {};                          \
        virtual ~Name() {};                                \
    };

#define DECLARE_STUN_ATTRIBUTE(Name, Type, Derives, Length) \
                                                            \
    class Name : public Derives                             \
    {                                                       \
    public:                                                 \
        static constexpr uint16_t TypeID = Type;            \
        Name(uint16_t size = Length)                        \
            : Derives(TypeID, size) {};                     \
        virtual ~Name() {};                                 \
    };


//
/// Attribute declarations
//

/// Address attributes
DECLARE_FIXLEN_STUN_ATTRIBUTE(MappedAddress, 0x0001, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ResponseAddress, 0x0002, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ChangedAddress, 0x0005, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ReflectedFrom, 0x000b, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(AlternateServer, 0x000e, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(SourceAddress, 0x0004, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(DestinationAddress, 0x0011, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(XorMappedAddress, 0x0020, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(XorPeerAddress, 0x0012, AddressAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(XorRelayedAddress, 0x0016, AddressAttribute)

/// uint32_t attributes
DECLARE_FIXLEN_STUN_ATTRIBUTE(Fingerprint, 0x8028, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(RequestedTransport, 0x0019, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ChangeRequest, 0x0003, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(Lifetime, 0x000d, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(Bandwidth, 0x0010, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(Options, 0x8001, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ChannelNumber, 0x000c, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ICEPriority, 0x0024, UInt32Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ConnectionID, 0x002a, UInt32Attribute)

/// UInt8 attributes
DECLARE_FIXLEN_STUN_ATTRIBUTE(EventPort, 0x0018, UInt8Attribute)

/// UInt32 list attributes
DECLARE_STUN_ATTRIBUTE(UnknownAttributes, 0x000a, UInt16ListAttribute, 0)

/// String attributes
DECLARE_STUN_ATTRIBUTE(Username, 0x0006, StringAttribute, 0)
DECLARE_STUN_ATTRIBUTE(Password, 0x0007, StringAttribute, 0)
DECLARE_STUN_ATTRIBUTE(MagicCookie, 0x000f, StringAttribute, 4)
DECLARE_STUN_ATTRIBUTE(Data, 0x0013, StringAttribute, 0)
DECLARE_STUN_ATTRIBUTE(Realm, 0x0014, StringAttribute, 0)
DECLARE_STUN_ATTRIBUTE(Nonce, 0x0015, StringAttribute, 0)
DECLARE_STUN_ATTRIBUTE(Software, 0x8022, StringAttribute, 0)
DECLARE_STUN_ATTRIBUTE(ReservationToken, 0x0022, StringAttribute, 8)

/// UInt64 attributes
DECLARE_FIXLEN_STUN_ATTRIBUTE(ICEControlling, 0x802A, UInt64Attribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(ICEControlled, 0x8029, UInt64Attribute)

/// Flag attributes
DECLARE_FIXLEN_STUN_ATTRIBUTE(ICEUseCandidate, 0x0025, FlagAttribute)
DECLARE_FIXLEN_STUN_ATTRIBUTE(DontFragment, 0x001A, FlagAttribute)
} // namespace stun
} // namespace icy


/// @\}
