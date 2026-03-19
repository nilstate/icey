///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "icy/symple/message.h"
#include "icy/logger.h"
#include "icy/util.h"

#include <stdexcept>


namespace icy {
namespace smpl {


Message::Message()
    : json::Value(json::Value::object())
{
    (*this)["id"] = util::randomString(16);
    (*this)["type"] = "message";

    if (!is_object())
        throw std::runtime_error("Message must be a JSON object");
}


Message::Message(const Message& root)
    : json::Value(static_cast<const json::Value&>(root))
    , IPacket()
{
    if (find("id") == end())
        (*this)["id"] = util::randomString(16);
    if (find("type") == end())
        (*this)["type"] = "message";

    if (!is_object())
        throw std::runtime_error("Message must be a JSON object");
}


Message::Message(const json::Value& root)
    : json::Value(root)
{
    if (find("id") == end())
        (*this)["id"] = util::randomString(16);
    if (find("type") == end())
        (*this)["type"] = "message";

    if (!is_object())
        throw std::runtime_error("Message must be a JSON object");
}


Message::~Message()
{
}


std::unique_ptr<IPacket> Message::clone() const
{
    return std::make_unique<Message>(*this);
}


ssize_t Message::read(const ConstBuffer& buf)
{
    return read(buf.str()); // refactor
}


ssize_t Message::read(const std::string& root)
{
    *this = json::Value::parse(root.begin(), root.end());
    return root.length();
}


void Message::write(Buffer& buf) const
{
    std::string data(dump());

    // buf.append(data.c_str(), data.size());
    buf.insert(buf.end(), data.begin(), data.end());
}


size_t Message::size() const
{
    return dump().size();
}


void Message::print(std::ostream& os) const
{
    os << dump(4);
}


bool Message::valid() const
{
    return find("type") != end() && find("id") != end();
    // && find("from") != end()
    // && (*this)["from"].get<std::string>().length()
}


void Message::clear()
{
    json::Value::clear();
}


void Message::clearData()
{
    (*this)["data"].clear();
}


void Message::clearNotes()
{
    (*this)["notes"].clear();
}


std::string Message::type() const
{
    return value("type", "message");
}


std::string Message::id() const
{
    return value("id", "");
}


Address Message::to() const
{
    return Address(value("to", ""));
}


Address Message::from() const
{
    return Address(value("from", ""));
}


int Message::status() const
{
    return find("status") != end() ? (*this)["status"].get<int>() : -1;
}


bool Message::isRequest() const
{
    return status() == -1;
}


json::Value& Message::notes()
{
    return (*this)["notes"];
}


json::Value Message::data(std::string_view name) const
{
    return (*this)["data"][std::string(name)];
}


json::Value& Message::data(std::string_view name)
{
    return (*this)["data"][std::string(name)];
}


void Message::setType(std::string_view type)
{
    (*this)["type"] = std::string(type);
}


void Message::setTo(const Peer& to)
{
    (*this)["to"] = to.address().toString();
}


void Message::setTo(const Address& to)
{
    (*this)["to"] = to.toString();
}


void Message::setTo(std::string_view to)
{
    (*this)["to"] = std::string(to);
}


void Message::setFrom(const Peer& from)
{
    (*this)["from"] = from.address().toString();
}


void Message::setFrom(const Address& from)
{
    if (!is_object())
        throw std::runtime_error("Message must be a JSON object");
    (*this)["from"] = from.toString();
}


void Message::setFrom(std::string_view from)
{
    (*this)["from"] = std::string(from);
}


void Message::setStatus(int code)
{
    if (code <= 100 || code >= 505)
        throw std::invalid_argument("HTTP status code out of range: " + std::to_string(code));
    (*this)["status"] = code;
}


void Message::setNote(std::string_view type, std::string_view text)
{
    clearNotes();
    addNote(type, text);
}

void Message::addNote(std::string_view type, std::string_view text)
{
    if (type != "info" && type != "warn" && type != "error")
        throw std::invalid_argument("Invalid note type: " + std::string(type));

    json::Value note;
    note["type"] = std::string(type);
    note["text"] = std::string(text);
    (*this)["notes"].push_back(note);
}


json::Value& Message::setData(std::string_view name)
{
    std::string key(name);
    return (*this)["data"][key] = key;
}


void Message::setData(std::string_view name, const char* data)
{
    (*this)["data"][std::string(name)] = data;
}


void Message::setData(std::string_view name, std::string_view data)
{
    (*this)["data"][std::string(name)] = std::string(data);
}


void Message::setData(std::string_view name, const json::Value& data)
{
    (*this)["data"][std::string(name)] = data;
}


void Message::setData(std::string_view name, int data)
{
    (*this)["data"][std::string(name)] = data;
}


void Message::removeData(std::string_view name)
{
    (*this)["data"].erase(std::string(name));
}


bool Message::hasData(std::string_view name)
{
    std::string key(name);
    auto& element = (*this)["data"];
    return element.find(key) != element.end();
}


} // namespace smpl
} // namespace icy


/// @\}
