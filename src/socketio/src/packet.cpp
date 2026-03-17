///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup socketio
/// @{


#include "scy/socketio/packet.h"
#include "scy/logger.h"
#include "scy/util.h"

#include <sstream>
#include <stdexcept>


namespace scy {
namespace sockio {


std::atomic<int> Packet::_idCounter{1};


int Packet::nextID()
{
    return _idCounter.fetch_add(1, std::memory_order_relaxed);
}


Packet::Packet(Frame frame, Type type, int id, const std::string& nsp,
               const std::string& event, const std::string& message, bool ack)
    : _frame(frame)
    , _type(type)
    , _id(id)
    , _nsp(nsp)
    , _event(event)
    , _message(message)
    , _ack(ack)
    , _size(0)
{
}


Packet::Packet(Type type, const std::string& message, bool ack)
    : Packet(Frame::Message, type, ack ? nextID() : -1, "/", "message", message, ack)
{
}


Packet::Packet(const std::string& message, bool ack)
    : Packet(Frame::Message, Type::Event, ack ? nextID() : -1, "/", "message", message, ack)
{
}


Packet::Packet(const json::value& message, bool ack)
    : Packet(Frame::Message, Type::Event, ack ? nextID() : -1, "/", "message", message.dump(), ack)
{
}


Packet::Packet(const std::string& event, const std::string& message, bool ack)
    : Packet(Frame::Message, Type::Event, ack ? nextID() : -1, "/", event, message, ack)
{
}


Packet::Packet(const std::string& event, const json::value& data, bool ack)
    : Packet(Frame::Message, Type::Event, ack ? nextID() : -1, "/", event, data.dump(), ack)
{
}


std::unique_ptr<IPacket> Packet::clone() const
{
    return std::make_unique<Packet>(*this);
}


ssize_t Packet::read(const ConstBuffer& buf)
{
    LTrace("Read raw packet: ", buf.str());

    if (buf.size() == 0)
        return 0;

    // Reset all data
    _frame = Frame::Unknown;
    _type = Type::Unknown;
    _id = -1;
    _nsp = "/";
    _event = "";
    _message = "";
    _size = 0;

    BitReader reader(buf);

    // Parse Engine.IO frame type (single digit)
    char frame[2] = {'\0'};
    reader.get(frame, 1);
    _frame = static_cast<Frame>(frame[0] - '0');

    // Only Message frames contain Socket.IO packet data
    if (_frame == Frame::Message) {

        // Parse Socket.IO packet type (single digit)
        char type[2] = {'\0'};
        reader.get(type, 1);
        _type = static_cast<Type>(type[0] - '0');

        // Parse binary attachment count (for BinaryEvent/BinaryAck)
        // Format: <count>-
        // Skip for now as binary is not yet implemented
        if (_type == Type::BinaryEvent || _type == Type::BinaryAck) {
            // Read past the attachment count and dash
            std::string attachments;
            while (reader.available() && reader.peek() != '-') {
                char c;
                reader.get(&c, 1);
                attachments += c;
            }
            if (reader.available() && reader.peek() == '-') {
                char dash;
                reader.get(&dash, 1); // consume the '-'
            }
        }

        // Parse namespace (starts with '/', ends at ',')
        if (reader.available() && reader.peek() == '/') {
            _nsp.clear();
            while (reader.available()) {
                char c;
                reader.get(&c, 1);
                if (c == ',')
                    break;
                _nsp += c;
            }
        }

        // Parse acknowledgement ID (sequence of digits)
        if (reader.available() && reader.peek() >= '0' && reader.peek() <= '9') {
            std::string idStr;
            while (reader.available() && reader.peek() >= '0' && reader.peek() <= '9') {
                char c;
                reader.get(&c, 1);
                idStr += c;
            }
            _id = std::stoi(idStr);
        }

        // Parse JSON payload
        if (reader.available()) {
            std::string temp;
            reader.get(temp, reader.available());

            json::value json = json::value::parse(temp.begin(), temp.end(), nullptr, false);
            if (json.is_array()) {
                if (json.size() < 2) {
                    _event = "message";
                    if (json.size() == 1) {
                        if (json[0].is_string())
                            _message = json[0].get<std::string>();
                        else
                            _message = json[0].dump();
                    }
                } else {
                    if (json[0].is_string())
                        _event = json[0].get<std::string>();
                    else
                        _event = "message";
                    _message = json[1].dump();
                }
            } else if (json.is_object()) {
                // Connect packets from server carry {"sid":"..."} as an object
                _message = json.dump();
            }
        }
    } else if (_frame == Frame::Open) {
        // Engine.IO open packet - the rest is JSON handshake data
        if (reader.available()) {
            std::string temp;
            reader.get(temp, reader.available());
            _message = temp;
        }
    }
    // Ping/Pong/Close/Upgrade/Noop have no nested Socket.IO type

    _size = reader.position();

    return static_cast<ssize_t>(_size);
}


void Packet::write(Buffer& buf) const
{
    std::ostringstream os;
    print(os);
    std::string str(os.str());
    buf.insert(buf.end(), str.begin(), str.end());
}


void Packet::setID(int id)
{
    _id = id;
}


void Packet::setNamespace(const std::string& nsp)
{
    _nsp = nsp;
}


void Packet::setEvent(const std::string& event)
{
    _event = event;
}


void Packet::setMessage(const std::string& message)
{
    _message = message;
}


void Packet::setAck(bool flag)
{
    _ack = flag;
}


Packet::Frame Packet::frame() const
{
    return _frame;
}


Packet::Type Packet::type() const
{
    return _type;
}


int Packet::id() const
{
    return _id;
}


std::string Packet::nsp() const
{
    return _nsp;
}


std::string Packet::event() const
{
    return _event;
}


std::string Packet::message() const
{
    return _message;
}


json::value Packet::json() const
{
    if (!_message.empty()) {
        auto result = json::value::parse(_message.begin(), _message.end(), nullptr, false);
        if (!result.is_discarded())
            return result;
    }
    return json::value();
}


std::string Packet::frameString() const
{
    switch (_frame) {
        case Frame::Open: return "Open";
        case Frame::Close: return "Close";
        case Frame::Ping: return "Ping";
        case Frame::Pong: return "Pong";
        case Frame::Message: return "Message";
        case Frame::Upgrade: return "Upgrade";
        case Frame::Noop: return "Noop";
        default: return "Unknown";
    }
}


std::string Packet::typeString() const
{
    switch (_type) {
        case Type::Connect: return "Connect";
        case Type::Disconnect: return "Disconnect";
        case Type::Event: return "Event";
        case Type::Ack: return "Ack";
        case Type::ConnectError: return "ConnectError";
        case Type::BinaryEvent: return "BinaryEvent";
        case Type::BinaryAck: return "BinaryAck";
        default: return "Unknown";
    }
}


std::string Packet::toString() const
{
    std::ostringstream ss;
    print(ss);
    return ss.str();
}


bool Packet::valid() const
{
    // Non-message frames (ping, pong, open, close, etc.) are always valid
    if (_frame != Frame::Message)
        return _frame != Frame::Unknown;

    // Message frames need a valid Socket.IO type
    int t = static_cast<int>(_type);
    if (t < 0 || t > 6)
        return false;

    // Event and Ack packets with ack flag need an ID
    if (_ack && _id < 0)
        return false;

    return true;
}


size_t Packet::size() const
{
    std::ostringstream ss;
    print(ss);
    return static_cast<size_t>(ss.tellp());
}


void Packet::print(std::ostream& os) const
{
    // Engine.IO frame prefix
    os << static_cast<int>(_frame);

    // Only Message frames carry Socket.IO content
    if (_frame != Frame::Message)
        return;

    // Socket.IO packet type
    os << static_cast<int>(_type);

    // Namespace (only for non-default namespaces)
    bool hasNsp = !_nsp.empty() && _nsp != "/";

    switch (_type) {
        case Type::Connect:
            // Format: 40 or 40/admin,{auth}
            if (hasNsp) {
                os << _nsp << ",";
            }
            if (!_message.empty()) {
                os << _message;
            }
            break;

        case Type::Disconnect:
            // Format: 41 or 41/admin,
            if (hasNsp) {
                os << _nsp << ",";
            }
            break;

        case Type::Event:
        case Type::BinaryEvent:
            // Format: 42["event",data] or 42/admin,["event",data]
            // With ack: 42<id>["event",data]
            if (hasNsp) {
                os << _nsp << ",";
            }
            if (_id >= 0) {
                os << _id;
            }
            os << "[\"" << (_event.empty() ? "message" : _event) << "\"";
            if (!_message.empty()) {
                os << "," << _message;
            }
            os << "]";
            break;

        case Type::Ack:
        case Type::BinaryAck:
            // Format: 43<id>[data] or 43/admin,<id>[data]
            if (hasNsp) {
                os << _nsp << ",";
            }
            if (_id >= 0) {
                os << _id;
            }
            if (!_message.empty()) {
                os << "[" << _message << "]";
            }
            break;

        case Type::ConnectError:
            // Format: 44{"message":"error"}
            if (hasNsp) {
                os << _nsp << ",";
            }
            if (!_message.empty()) {
                os << _message;
            }
            break;

        default:
            break;
    }
}


} // namespace sockio
} // namespace scy


/// @}
