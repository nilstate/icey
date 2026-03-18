///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup base
/// @{


#pragma once


#include "scy/base.h"
#include "scy/bitwise.h"
#include "scy/buffer.h"
#include "scy/interface.h"
#include "scy/logger.h"

#include <cstdint>
#include <cstring> // memcpy
#include <memory>


namespace scy {


/// An abstract interface for packet sources to
/// provide extra information about packets.
struct IPacketInfo
{
    IPacketInfo() = default;
    virtual ~IPacketInfo() = default;

    virtual std::unique_ptr<IPacketInfo> clone() const = 0;
};


/// The basic packet type which is passed around the LibSourcey system.
/// IPacket can be extended for each protocol to enable polymorphic
/// processing and callbacks using PacketStream and friends.
class Base_API IPacket
{
public:
    IPacket(void* opaque = nullptr,
            std::unique_ptr<IPacketInfo> info = nullptr, unsigned flags = 0)
        : opaque(opaque)
        , info(std::move(info))
        , flags(flags)
    {
    }

    IPacket(const IPacket& r)
        : opaque(r.opaque)
        , info(r.info ? r.info->clone() : nullptr)
        , flags(r.flags)
    {
    }

    IPacket& operator=(const IPacket& r)
    {
        opaque = r.opaque;
        info = r.info ? r.info->clone() : nullptr;
        flags = r.flags;
        return *this;
    }

    virtual std::unique_ptr<IPacket> clone() const = 0;

    virtual ~IPacket() = default;

    /// Optional client data pointer.
    /// This pointer is not managed by the packet.
    void* opaque;

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

    virtual bool hasData() const { return data() != nullptr; }

    /// The packet data pointer for buffered packets.
    virtual char* data() const { return nullptr; }

    /// The const packet data pointer for buffered packets.
    virtual const void* constData() const { return data(); }
    virtual const char* className() const = 0;
    virtual void print(std::ostream& os) const
    {
        os << className() << '\n';
    }

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
    FlagPacket(unsigned flags = 0)
        : IPacket(nullptr, nullptr, flags)
    {
    }

    virtual std::unique_ptr<IPacket> clone() const override
    {
        return std::make_unique<FlagPacket>(*this);
    }

    FlagPacket(const FlagPacket& that)
        : IPacket(that)
    {
    }

    virtual ~FlagPacket() = default;

    virtual ssize_t read(const ConstBuffer&) override { return true; }

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
              void* opaque = nullptr,
              std::unique_ptr<IPacketInfo> info = nullptr)
        : IPacket(opaque, std::move(info), flags)
        , _data(data)
        , _size(size)
    {
    }

    /// Construct with const data (copied, owning).
    RawPacket(const char* data, size_t size = 0, unsigned flags = 0,
              void* opaque = nullptr,
              std::unique_ptr<IPacketInfo> info = nullptr)
        : IPacket(opaque, std::move(info), flags)
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

    virtual std::unique_ptr<IPacket> clone() const override
    {
        return std::make_unique<RawPacket>(*this);
    }

    virtual void copyData(const void* data, size_t size)
    {
        if (data && size > 0) {
            _owned = std::make_unique<char[]>(size);
            _data = _owned.get();
            _size = size;
            std::memcpy(_data, data, size);
        }
    }

    virtual ssize_t read(const ConstBuffer& buf) override
    {
        copyData(bufferCast<const char*>(buf), buf.size());
        return buf.size();
    }

    virtual void write(Buffer& buf) const override
    {
        buf.insert(buf.end(), _data, _data + _size);
    }

    virtual char* data() const override { return _data; }

    virtual size_t size() const override { return _size; }

    virtual const char* className() const override { return "RawPacket"; }

    bool ownsBuffer() const { return _owned != nullptr; }

protected:
    char* _data;
    size_t _size;
    std::unique_ptr<char[]> _owned;
};


inline RawPacket rawPacket(const MutableBuffer& buf, unsigned flags = 0,
                           void* opaque = nullptr,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(bufferCast<char*>(buf), buf.size(), flags, opaque, std::move(info));
}

inline RawPacket rawPacket(const ConstBuffer& buf, unsigned flags = 0,
                           void* opaque = nullptr,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(bufferCast<const char*>(buf), buf.size(), flags, opaque, std::move(info));
}

inline RawPacket rawPacket(char* data = nullptr, size_t size = 0,
                           unsigned flags = 0, void* opaque = nullptr,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(data, size, flags, opaque, std::move(info));
}

inline RawPacket rawPacket(const char* data = nullptr, size_t size = 0,
                           unsigned flags = 0, void* opaque = nullptr,
                           std::unique_ptr<IPacketInfo> info = nullptr)
{
    return RawPacket(data, size, flags, opaque, std::move(info));
}


} // namespace scy
