///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <https://sourcey.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#include "scy/symple/message.h"
#include "scy/logger.h"
#include "scy/util.h"

#include <stdexcept>


namespace scy {
namespace smpl {


Message::Message()
    : json::value(json::value::object())
{
    (*this)["id"] = util::randomString(16);
    (*this)["type"] = "message";

    if (!is_object())
        throw std::runtime_error("Message must be a JSON object");
}


Message::Message(const Message& root)
    : json::value(static_cast<const json::value&>(root))
{
    if (find("id") == end())
        (*this)["id"] = util::randomString(16);
    if (find("type") == end())
        (*this)["type"] = "message";

    if (!is_object())
        throw std::runtime_error("Message must be a JSON object");
}


Message::Message(const json::value& root)
    : json::value(root)
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
    *this = json::value::parse(root.begin(), root.end());
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
    json::value::clear();
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


json::value& Message::notes()
{
    return (*this)["notes"];
}


json::value Message::data(const std::string& name) const
{
    return (*this)["data"][name];
}


json::value& Message::data(const std::string& name)
{
    return (*this)["data"][name];
}


void Message::setType(const std::string& type)
{
    (*this)["type"] = type;
}


void Message::setTo(const Peer& to)
{
    (*this)["to"] = to.address().toString();
}


void Message::setTo(const Address& to)
{
    (*this)["to"] = to.toString();
}


void Message::setTo(const std::string& to)
{
    (*this)["to"] = to;
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


void Message::setFrom(const std::string& from)
{
    (*this)["from"] = from;
}


void Message::setStatus(int code)
{
    if (code <= 100 || code >= 505)
        throw std::invalid_argument("HTTP status code out of range: " + std::to_string(code));
    (*this)["status"] = code;
}


void Message::setNote(const std::string& type, const std::string& text)
{
    clearNotes();
    addNote(type, text);
}

void Message::addNote(const std::string& type, const std::string& text)
{
    if (type != "info" && type != "warn" && type != "error")
        throw std::invalid_argument("Invalid note type: " + type);

    json::value note;
    note["type"] = type;
    note["text"] = text;
    (*this)["notes"].push_back(note);
}


json::value& Message::setData(const std::string& name)
{
    return (*this)["data"][name] = name;
}


void Message::setData(const std::string& name, const char* data)
{
    (*this)["data"][name] = data;
}


void Message::setData(const std::string& name, const std::string& data)
{
    (*this)["data"][name] = data;
}


void Message::setData(const std::string& name, const json::value& data)
{
    (*this)["data"][name] = data;
}


void Message::setData(const std::string& name, int data)
{
    (*this)["data"][name] = data;
}


void Message::removeData(const std::string& name)
{
    (*this)["data"].erase(name);
}


bool Message::hasData(const std::string& name)
{
    auto& element = (*this)["data"];
    return element.find(name) != element.end();
}


} // namespace smpl
} // namespace scy


/// @\}
