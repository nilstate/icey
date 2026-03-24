///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "icy/base.h"
#include "icy/bitwise.h"
#include "icy/buffer.h"
#include "icy/interface.h"
#include "icy/logger.h"

#include <any>

#include <cstdint>
#include <cstring> // memcpy
#include <memory>


namespace icy {


/// An abstract interface for packet sources to
/// provide extra information about packets.
struct IPacketInfo
{
    IPacketInfo() = default;
    virtual ~IPacketInfo() = default;

    /// Returns a heap-allocated deep copy of this info object.
    /// @return Owning pointer to the cloned instance.
    virtual std::unique_ptr<IPacketInfo> clone() const = 0;
};


/// The basic packet type which is passed around the Icey system.
/// IPacket can be extended for each protocol to enable polymorphic
/// processing and callbacks using PacketStream and friends.
class Base_API IPacket
{
public:
    /// @param info Optional packet info; ownership transferred.
    /// @param flags Initial bitwise flags.
    IPacket(std::unique_ptr<IPacketInfo> info = nullptr, unsigned flags = 0)
        : info(std::move(info))
        , flags(flags)
    {
    }

    /// Copy constructor; clones the info object if present.
    /// @param r Source packet.
    IPacket(const IPacket& r)
        : opaque(r.opaque)
        , info(r.info ? r.info->clone() : nullptr)
        , flags(r.flags)
    {
    }

    /// Copy assignment; clones the info object if present.
    /// @param r Source packet.
    /// @return Reference to this packet.
    IPacket& operator=(const IPacket& r)
    {
        opaque = r.opaque;
        info = r.info ? r.info->clone() : nullptr;
        flags = r.flags;
        return *this;
    }

    /// Returns a heap-allocated deep copy of this packet.
    /// @return Owning pointer to the cloned packet.
    virtual std::unique_ptr<IPacket> clone() const = 0;

    virtual ~IPacket() = default;

    /// Optional type-safe context data. Use std::any_cast to retrieve.
    /// Lifetime of the stored value is tied to the packet's lifetime.
    std::any opaque;

    /// Optional extra information about the packet.
    std::unique_ptr<IPacketInfo> info;

    /// Provides basic information about the packet.
    Bitwise flags;

    /// Read/parse to the packet from the given input buffer.
    /// The number of bytes read is returned.
    virtual ssize_t read(const ConstBuffer&) = 0;

    /// Copy/generate to the packet given output buffer.
    /// The number of bytes written can be obtained from the buffer.
    ///
    /// Todo: It may be prefferable to use our pod types here
    /// instead of buffer input, but the current codebase requires
    /// that the buffer be dynamically resizable for some protocols...
    ///
    virtual void write(Buffer&) const = 0;

    /// The size of the packet in bytes.
    ///
    /// This is the nember of bytes that will be written on a call
    /// to write(), but may not be the number of bytes that will be
    /// consumed by read().
    virtual size_t size() const { return 0; };

    /// Returns true if the packet has a non-null data pointer.
    virtual bool hasData() const { return data() != nullptr; }

    /// The packet data pointer for buffered packets.
    virtual char* data() const { return nullptr; }

    /// The const packet data pointer for buffered packets.
    virtual const void* constData() const { return data(); }

    /// Returns the class name of this packet type for logging and diagnostics.
    virtual const char* className() const = 0;

    /// Prints a human-readable representation to the given stream.
    /// @param os Output stream.
    virtual void print(std::ostream& os) const
    {
        os << className() << '\n';
    }

    /// Stream insertion operator; delegates to print().
    /// @param stream Output stream.
    /// @param p Packet to print.
    /// @return Reference to the stream.
    friend std::ostream& operator<<(std::ostream& stream, const IPacket& p)
    {
        p.print(stream);
        return stream;
    }
};


/// Packet for sending bitwise flags along the packet stream.
class Base_API FlagPacket : public IPacket
{
public:
    /// @param flags Bitwise flags to carry in this packet.
    FlagPacket(unsigned flags = 0)
        : IPacket(nullptr, flags)
    {
    }

    /// @return Owning pointer to a deep copy of this packet.
    virtual std::unique_ptr<IPacket> clone() const override
    {
        return std::make_unique<FlagPacket>(*this);
    }

    /// @param that Source packet to copy from.
    FlagPacket(const FlagPacket& that)
        : IPacket(that)
    {
    }

    virtual ~FlagPacket() = default;

    /// No-op read; FlagPacket carries no payload data.
    /// @return Always returns true (1).
    virtual ssize_t read(const ConstBuffer&) override { return true; }

    /// No-op write; FlagPacket carries no payload data.
    virtual void write(Buffer&) const override {}

    virtual const char* className() const override { return "FlagPacket"; }
};


/// RawPacket is the default data packet type which consists
/// of an optionally managed char pointer and a size value.
class Base_API RawPacket : public IPacket
{
public:
    /// Construct with borrowed (non-owning) buffer.
    RawPacket(char* data = nullptr, size_t size = 0, unsigned flags = 0,
              std::unique_ptr<IPacketInfo> info = nullptr)
        : IPacket(std::move(info), flags)
        , _data(data)
        , _size(size)
    {
    }

    /// Construct with const data (copied, owning).
    RawPacket(const char* data, size_t size = 0, unsigned flags = 0,
              std::unique_ptr<IPacketInfo> info = nullptr)
        : IPacket(std::move(info), flags)
        , _data(nullptr)
        , _size(0)
    {
        copyData(data, size);
    }

    /// Copy constructor (always copies data).
    RawPacket(const RawPacket& that)
        : IPacket(that)
        , _data(nullptr)
        , _size(0)
    {
        copyData(that._data, that._size);
    }

    virtual ~RawPacket() = default;

    /// @return Owning pointer to a deep copy of this packet (always copies data).
    virtual std::unique_ptr<IPacket> clone() const override
    {
        return std::make_unique<RawPacket>(*this);
    }

    /// Copies data into an internally owned buffer, replacing any prior content.
    /// @param data Source data pointer.
    /// @param size Number of bytes to copy.
    virtual void copyData(const void* data, size_t size)
    {
        if (data && size > 0) {
            _owned = std::make_unique<char[]>(size);
            _data = _owned.get();
            _size = size;
            std::memcpy(_data, data, size);
        }
    }

    /// Reads from the buffer by copying its contents into an owned buffer.
    /// @param buf Input buffer to read from.
    /// @return Number of bytes consumed (equal to buf.size()).
    virtual ssize_t read(const ConstBuffer& buf) override
    {
        copyData(bufferCast<const char*>(buf), buf.size());
        return buf.size();
    }

    /// Appends the packet data to the given output buffer.
    /// @param buf Buffer to write into.
    virtual void write(Buffer& buf) const override
    {
        buf.insert(buf.end(), _data, _data + _size);
    }

    /// @return Mutable pointer to the raw packet data, or nullptr if empty.
    virtual char* data() const override { return _data; }

    /// @return Size of the packet data in bytes.
    virtual size_t size() const override { return _size; }

    virtual const char* className() const override { return "RawPacket"; }

    /// @return True if this packet owns (manages) its data buffer.
    bool ownsBuffer() const { return _owned != nullptr; }

protected:
    char* _data;
    size_t _size;
    std::unique_ptr<char[]> _owned;
};


/// Constructs a non-owning RawPacket from a mutable buffer (borrowed pointer).
inline RawPacket rawPacket(const MutableBuffer& buf, unsigned flags = 0,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(bufferCast<char*>(buf), buf.size(), flags, std::move(info));
}

/// Constructs an owning RawPacket from a const buffer (data is copied).
inline RawPacket rawPacket(const ConstBuffer& buf, unsigned flags = 0,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(bufferCast<const char*>(buf), buf.size(), flags, std::move(info));
}

/// Constructs a non-owning RawPacket from a raw mutable pointer (borrowed).
inline RawPacket rawPacket(char* data = nullptr, size_t size = 0,
                           unsigned flags = 0,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(data, size, flags, std::move(info));
}

/// Constructs an owning RawPacket from a const char pointer (data is copied).
inline RawPacket rawPacket(const char* data = nullptr, size_t size = 0,
                           unsigned flags = 0,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(data, size, flags, std::move(info));
}


} // namespace icy

/// @}
