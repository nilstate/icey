///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{

#pragma once

#include "scy/json/json.h"
#include "scy/packet.h"
#include "scy/socketio/socketio.h"

#include <atomic>
#include <string>


namespace scy {
namespace sockio {


/// Engine.IO frame types (protocol v4).
enum class Frame : int
{
    Open = 0,
    Close = 1,
    Ping = 2,
    Pong = 3,
    Message = 4,
    Upgrade = 5,
    Noop = 6,
    Unknown = -1
};

/// Socket.IO packet types (protocol v5).
enum class Type : int
{
    Connect = 0,
    Disconnect = 1,
    Event = 2,
    Ack = 3,
    ConnectError = 4,
    BinaryEvent = 5,
    BinaryAck = 6,
    Unknown = -1
};


class SocketIO_API Packet : public IPacket
{
public:
    // Keep nested aliases for backward compatibility with existing code
    // that uses Packet::Frame and Packet::Type
    using Frame = sockio::Frame;
    using Type = sockio::Type;

    /// Default constructor
    Packet(Frame frame = Frame::Message, Type type = Type::Event, int id = -1,
           const std::string& nsp = "/", const std::string& event = "",
           const std::string& message = "", bool ack = false);

    /// General constructor
    Packet(Type type, const std::string& message = "", bool ack = false);

    /// Message constructor
    Packet(const std::string& message, bool ack = false);

    /// JSON constructor
    Packet(const json::value& message, bool ack = false);

    /// Event constructor
    Packet(const std::string& event, const std::string& message, bool ack = false);

    /// Event JSON constructor
    Packet(const std::string& event, const json::value& message, bool ack = false);

    Packet(const Packet& r) = default;
    Packet& operator=(const Packet& r) = default;
    Packet(Packet&& r) noexcept = default;
    Packet& operator=(Packet&& r) noexcept = default;
    ~Packet() override = default;

    std::unique_ptr<IPacket> clone() const override;

    [[nodiscard]] Frame frame() const;
    [[nodiscard]] Type type() const;
    [[nodiscard]] int id() const;
    [[nodiscard]] std::string nsp() const;
    [[nodiscard]] std::string event() const;
    [[nodiscard]] std::string message() const;
    [[nodiscard]] json::value json() const;

    void setID(int id);
    void setNamespace(const std::string& nsp);
    void setEvent(const std::string& event);
    void setMessage(const std::string& message);
    void setAck(bool flag);

    ssize_t read(const ConstBuffer& buf) override;
    void write(Buffer& buf) const override;

    [[nodiscard]] size_t size() const override;

    /// A packet is valid if it has a known frame and type.
    /// Connect/Disconnect packets don't require an ID.
    [[nodiscard]] bool valid() const;

    [[nodiscard]] std::string frameString() const;
    [[nodiscard]] std::string typeString() const;
    [[nodiscard]] std::string toString() const;
    void print(std::ostream& os) const;

    const char* className() const override { return "SocketIO::Packet"; }

    /// Return the next sequential packet ID (thread-safe).
    static int nextID();

protected:
    Frame _frame;
    Type _type;
    int _id;
    std::string _nsp;
    std::string _event;
    std::string _message;
    bool _ack;
    size_t _size;

    static std::atomic<int> _idCounter;
};


} // namespace sockio
} // namespace scy


/// @}
